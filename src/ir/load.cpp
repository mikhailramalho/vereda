#include "ir/load.h"

#include <llvm/ADT/StringExtras.h>
#include <llvm/IR/AttributeMask.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Operator.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Linker/Linker.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/Regex.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Support/SourceMgr.h>

using namespace llvm;

namespace vereda::ir {

// Poison-generating flags, attributes and metadata let LLVM assume no
// undefined behaviour; the walker must not. Strip them before InstCombine
// (which would fold checks with them) and after (which may infer them back,
// as inferattrs infers mustprogress).
static void stripUBAssumptions(Module &m) {
  AttributeMask ubAttrs;
  for (auto k : {Attribute::NoUndef, Attribute::NonNull, Attribute::Dereferenceable,
                 Attribute::DereferenceableOrNull, Attribute::Range, Attribute::OptimizeNone,
                 Attribute::MustProgress})
    ubAttrs.addAttribute(k);
  for (Function &f : m) {
    f.removeFnAttrs(ubAttrs);
    f.removeRetAttrs(ubAttrs);
    for (unsigned i = 0; i < f.arg_size(); ++i)
      f.removeParamAttrs(i, ubAttrs);
    for (Instruction &inst : instructions(f)) {
      if (isa<OverflowingBinaryOperator>(inst) || isa<TruncInst>(inst)) {
        inst.setHasNoUnsignedWrap(false);
        inst.setHasNoSignedWrap(false);
      }
      if (isa<PossiblyExactOperator>(inst))
        inst.setIsExact(false);
      if (isa<PossiblyNonNegInst>(inst))
        inst.setNonNeg(false);
      if (auto *d = dyn_cast<PossiblyDisjointInst>(&inst))
        d->setIsDisjoint(false);
      if (auto *c = dyn_cast<ICmpInst>(&inst))
        c->setSameSign(false);
      if (auto *gep = dyn_cast<GetElementPtrInst>(&inst))
        gep->setNoWrapFlags(GEPNoWrapFlags::none());
      if (auto *call = dyn_cast<CallBase>(&inst)) {
        call->removeFnAttrs(ubAttrs);
        call->removeRetAttrs(ubAttrs);
        for (unsigned i = 0; i < call->arg_size(); ++i)
          call->removeParamAttrs(i, ubAttrs);
      }
      for (unsigned md : {LLVMContext::MD_range, LLVMContext::MD_nonnull,
                          LLVMContext::MD_noundef, LLVMContext::MD_dereferenceable,
                          LLVMContext::MD_dereferenceable_or_null, LLVMContext::MD_align})
        inst.setMetadata(md, nullptr);
    }
  }
}

// Whether a value of type from can stand for one of type to, as in CBMC's
// function_is_type_compatible: integers and pointers of any width for each
// other, other types of the same size bit for bit.
static bool convertible(Type *from, Type *to, const DataLayout &dl) {
  auto scalar = [](Type *ty) { return ty->isIntegerTy() || ty->isPointerTy(); };
  if (from == to || (scalar(from) && scalar(to)))
    return true;
  return from->isSized() && to->isSized() && !from->isAggregateType() &&
         !to->isAggregateType() && dl.getTypeSizeInBits(from) == dl.getTypeSizeInBits(to);
}

static Value *convert(IRBuilder<> &b, Value *v, Type *to, const DataLayout &dl) {
  if (v->getType() == to)
    return v;
  // Through an integer: a pointer by its address, anything else by its bits.
  if (v->getType()->isPointerTy())
    v = b.CreatePtrToInt(v, dl.getIntPtrType(v->getType()));
  else if (!v->getType()->isIntegerTy())
    v = b.CreateBitCast(v, b.getIntNTy(dl.getTypeSizeInBits(v->getType())));
  if (to->isPointerTy())
    return b.CreateIntToPtr(v, to);
  if (to->isIntegerTy())
    return b.CreateZExtOrTrunc(v, to);
  return b.CreateBitCast(b.CreateZExtOrTrunc(v, b.getIntNTy(dl.getTypeSizeInBits(to))), to);
}

// Whether call c may run f: the same type, the same number of parameters
// each convertible, or no parameters at all, as a definition without a
// prototype has. The result must convert too unless unused. A variadic call
// is taken as typed by its arguments, as CBMC does: through a pointer
// without a prototype, clang calls with type (...).
static bool compatible(const CallInst &c, const Function &f, const DataLayout &dl) {
  FunctionType *call = c.getFunctionType(), *ft = f.getFunctionType();
  if (call == ft)
    return true;
  if (f.isIntrinsic() || ft->isVarArg())
    return false;
  if (!call->getReturnType()->isVoidTy() && !c.use_empty() &&
      (ft->getReturnType()->isVoidTy() ||
       !convertible(ft->getReturnType(), call->getReturnType(), dl)))
    return false;
  if (ft->getNumParams() == 0)
    return true;
  if (ft->getNumParams() != c.arg_size() ||
      (!call->isVarArg() && call->getNumParams() != c.arg_size()))
    return false;
  for (unsigned i = 0; i < ft->getNumParams(); ++i)
    if (!convertible(c.getArgOperand(i)->getType(), ft->getParamType(i), dl))
      return false;
  return true;
}

// A call to f with c's arguments, converted to f's parameters, and its
// result converted to c's type, null when void, unused or never returned.
// A call that does not return stays last before its block's end, where the
// walker looks for it.
static std::pair<CallInst *, Value *> callAs(IRBuilder<> &b, CallInst &c, Function &f,
                                             const DataLayout &dl) {
  CallInst *direct;
  if (c.getFunctionType() == f.getFunctionType()) {
    direct = cast<CallInst>(c.clone());
    direct->setCalledOperand(&f);
    direct->setTailCallKind(CallInst::TCK_None); // no longer before its ret
    b.Insert(direct);
  } else {
    SmallVector<Value *, 4> args;
    for (unsigned i = 0; i < f.arg_size(); ++i) // extra arguments are dropped
      args.push_back(convert(b, c.getArgOperand(i), f.getArg(i)->getType(), dl));
    direct = b.CreateCall(&f, args);
  }
  if (c.doesNotReturn() || f.doesNotReturn()) {
    direct->setDoesNotReturn();
    return {direct, nullptr};
  }
  if (c.getType()->isVoidTy() || f.getReturnType()->isVoidTy())
    return {direct, nullptr};
  return {direct, convert(b, direct, c.getType(), dl)};
}

// Indirect calls become direct ones, as CBMC's remove_function_pointers
// pass: p(args) becomes a chain of p == @f tests, one per address-taken
// function the call may run (see compatible), each calling f directly, and a
// phi for the result. A p that is none of them is a function this program
// does not define, as ESBMC's open world has it: the call goes to a function
// without a body, so its result is unconstrained. The walker folds the
// tests: a pointer known to be one of a few functions walks only those.
// A direct call to a defined function of another type is converted the same
// way.
static void lowerCalls(Module &m) {
  const DataLayout &dl = m.getDataLayout();
  DenseMap<FunctionType *, Function *> unknown; // one per type, so one warning
  SmallVector<CallInst *, 8> calls;
  for (Function &f : m)
    for (Instruction &inst : instructions(f))
      if (auto *c = dyn_cast<CallInst>(&inst))
        if (!c->isInlineAsm() && !c->getCalledFunction())
          calls.push_back(c);
  for (CallInst *c : calls) {
    IRBuilder<> b(c);
    if (auto *f = dyn_cast<Function>(c->getCalledOperand()->stripPointerCasts())) {
      if (f->isDeclaration() || !compatible(*c, *f, dl))
        continue; // bodyless ones go by name; the walker rejects the others
      Value *r = callAs(b, *c, *f, dl).second;
      if (!c->use_empty())
        c->replaceAllUsesWith(r ? r : PoisonValue::get(c->getType()));
      c->eraseFromParent();
      continue;
    }
    BasicBlock *head = c->getParent();
    BasicBlock *tail = head->splitBasicBlock(c, head->getName() + ".called");
    head->getTerminator()->eraseFromParent();
    b.SetInsertPoint(head);
    PHINode *result = nullptr;
    if (!c->getType()->isVoidTy())
      result = PHINode::Create(c->getType(), 0, "", tail->begin());
    auto branch = [&](Function &f, BasicBlock *from) {
      b.SetInsertPoint(from);
      auto [direct, r] = callAs(b, *c, f, dl);
      if (direct->doesNotReturn()) {
        b.CreateUnreachable();
        return;
      }
      b.CreateBr(tail);
      if (result)
        result->addIncoming(r ? r : PoisonValue::get(c->getType()), b.GetInsertBlock());
    };
    LLVMContext &ctx = m.getContext();
    for (Function &f : m) {
      if (!f.hasAddressTaken(nullptr, false, true, false, false, /*IgnoreCastedDirectCall=*/true) || !compatible(*c, f, dl))
        continue;
      BasicBlock *call = BasicBlock::Create(ctx, "call." + f.getName(), head->getParent(), tail);
      BasicBlock *next = BasicBlock::Create(ctx, "", head->getParent(), tail);
      b.CreateCondBr(b.CreateICmpEQ(c->getCalledOperand(), &f), call, next);
      branch(f, call);
      b.SetInsertPoint(next);
    }
    Function *&none = unknown[c->getFunctionType()];
    if (!none)
      none = Function::Create(c->getFunctionType(), GlobalValue::ExternalLinkage,
                              "__vereda_unknown_target", m);
    branch(*none, b.GetInsertBlock());
    if (result)
      c->replaceAllUsesWith(result);
    c->eraseFromParent();
  }
}

// Calls to the libm functions LLVM has an intrinsic for become that
// intrinsic when the program gives no body, so the encoder computes them.
// errno is not set; clang keeps sqrt and fma as calls only for it.
static void libmToIntrinsics(Module &m) {
  static const std::pair<const char *, Intrinsic::ID> names[] = {
      {"sqrt", Intrinsic::sqrt},   {"fabs", Intrinsic::fabs},
      {"floor", Intrinsic::floor}, {"ceil", Intrinsic::ceil},
      {"trunc", Intrinsic::trunc}, {"round", Intrinsic::round},
      {"rint", Intrinsic::rint},   {"nearbyint", Intrinsic::nearbyint},
      {"fmin", Intrinsic::minnum}, {"fmax", Intrinsic::maxnum},
      {"fma", Intrinsic::fma},     {"copysign", Intrinsic::copysign},
      {"roundeven", Intrinsic::roundeven}};
  for (auto [base, id] : names)
    for (const char *suffix : {"", "f", "l"}) {
      Function *f = m.getFunction(std::string(base) + suffix);
      if (!f || !f->isDeclaration() || f->isVarArg() || !f->getReturnType()->isFloatingPointTy() ||
          any_of(f->getFunctionType()->params(),
                 [&](Type *p) { return p != f->getReturnType(); }))
        continue;
      Function *intrinsic =
          Intrinsic::getOrInsertDeclaration(&m, id, {f->getReturnType()});
      if (intrinsic->arg_size() != f->arg_size())
        continue;
      for (User *u : make_early_inc_range(f->users()))
        if (auto *c = dyn_cast<CallInst>(u);
            c && c->getCalledOperand() == f && c->getFunctionType() == f->getFunctionType())
          c->setCalledFunction(intrinsic);
    }
}

// The instructions an inline asm runs where it stands, joined by " ; ":
// without labels, lock prefixes (one thread), or what it puts in other
// sections, as the kernel does for its bug table, exception table and lock
// lists. Blanks go but the one after the mnemonic, ${0:q} is $0, and the
// mnemonics the idioms below use lose their size suffix (incl is inc); bits
// is the first instruction's operand size as its suffix or a register
// modifier (${0:k}) gives it, else 0.
static std::string inlineInstructions(StringRef s, unsigned &bits) {
  static const Regex modifier("\\$\\{([0-9]+):([a-zA-Z]+)\\}");
  std::string out;
  bool away = false;
  bits = 0;
  SmallVector<StringRef, 8> stmts;
  SplitString(s, stmts, "\n;");
  for (StringRef stmt : stmts) {
    SmallVector<StringRef, 4> words;
    SplitString(stmt, words, " \t");
    ArrayRef<StringRef> w(words);
    while (!w.empty() && (w[0].ends_with(":") || w[0] == "lock")) // labels and lock prefixes
      w = w.drop_front();
    if (w.empty())
      continue;
    if (w[0] == ".section" || w[0] == ".pushsection") {
      away = true;
    } else if (w[0] == ".previous" || w[0] == ".popsection" || w[0] == ".text") {
      away = false;
    } else if (!away) {
      StringRef mn = w[0];
      size_t size = StringRef("bwlq").find(mn.back());
      if (size != StringRef::npos &&
          is_contained({"inc", "dec", "add", "sub", "xadd", "xchg", "cmpxchg", "bts", "btr",
                        "sbb", "mov", "bswap"},
                       mn.drop_back())) {
        mn = mn.drop_back();
        if (out.empty())
          bits = 8 << size;
      }
      std::string ops = join(w.drop_front(), "");
      SmallVector<StringRef, 3> mod;
      if (out.empty() && !bits && modifier.match(ops, &mod) && mod[2].size() == 1 &&
          StringRef("bwkq").contains(mod[2][0])) // a register of that size, as a suffix says
        bits = 8 << StringRef("bwkq").find(mod[2][0]);
      while (modifier.match(ops))
        ops = modifier.sub("$\\1", ops);
      out += (out.empty() ? "" : " ; ") + mn.str() + (ops.empty() ? "" : " " + ops);
    }
  }
  return out;
}

// Kernel atomics and bit operations (asm/atomic.h, bitops.h, cmpxchg.h) as
// written there, with each operand's constraint as "=v" (a register output),
// "v" (a register or constant input), "=*m" and "*m" (memory), or a digit
// (an input tied to that output). Each reads, changes and writes the memory
// at its first argument, which in one thread is an atomicrmw; then some set
// a flag: whether the result is zero (sete, into memory at argument 1), or
// the old bit (setc, the same; sbb, as the result 0 or -1). value is the
// argument added, subtracted, swapped in or giving the bit, or -1 for 1.
struct KernelAtomic {
  const char *code, *ops;
  int value;
  char flag; // 0, 'z' (sete), 'c' (setc), 's' (sbb), or 'o': the old value is the result
};
static const KernelAtomic kernelAtomics[] = {
    {"inc $0", "=*m,*m", -1, 0},
    {"dec $0", "=*m,*m", -1, 0},
    {"add $1,$0", "=*m,v,*m", 1, 0},
    {"sub $1,$0", "=*m,v,*m", 1, 0},
    {"inc $0 ; sete $1", "=*m,=*m,*m", -1, 'z'},
    {"dec $0 ; sete $1", "=*m,=*m,*m", -1, 'z'},
    {"add $2,$0 ; sete $1", "=*m,=*m,v,*m", 2, 'z'},
    {"sub $2,$0 ; sete $1", "=*m,=*m,v,*m", 2, 'z'},
    {"bts $1,$0", "=*m,v,*m", 1, 0},
    {"btr $1,$0", "=*m,v,*m", 1, 0},
    {"bts $2,$1 ; sbb $0,$0", "=v,=*m,v,*m", 1, 's'},
    {"btr $2,$1 ; sbb $0,$0", "=v,=*m,v,*m", 1, 's'},
    {"bts $2,$0 ; setc $1", "=*m,=*m,v,*m", 2, 'c'},
    {"btr $2,$0 ; setc $1", "=*m,=*m,v,*m", 2, 'c'},
    {"xchg $0,$1", "=v,=*m,0,*m", 1, 'o'},
    {"xadd $0,$1", "=v,=*m,0,*m", 1, 'o'},
};

// Whether the program expects no return to next: it is unreachable, or a
// branch to an empty endless loop, as CIL writes BUG()'s unreachable().
static bool noReturn(Instruction *next) {
  if (isa<UnreachableInst>(next))
    return true;
  auto *br = dyn_cast<BranchInst>(next);
  if (!br || br->isConditional())
    return false;
  auto *loop = dyn_cast<BranchInst>(&br->getSuccessor(0)->front());
  return loop && loop->isUnconditional() && loop->getSuccessor(0) == loop->getParent();
}

// The IR for an asm in kernelAtomics (code and shape as there, ops its
// constraints, bits as inlineInstructions gives), before c; null, with
// nothing emitted, when the types do not fit.
static Value *lowerKernelAtomic(CallInst *c, StringRef code, StringRef shape, StringRef ops,
                                unsigned bits) {
  const KernelAtomic *k = find_if(
      kernelAtomics, [&](const KernelAtomic &k) { return code == k.code && shape == k.ops; });
  if (k == std::end(kernelAtomics))
    return nullptr;
  StringRef mn = code.take_until([](char ch) { return ch == ' '; });
  bool bit = mn.starts_with("bt");
  Type *ty = c->getType(), *t = c->getParamElementType(0);
  Type *flagTy = k->flag == 'z' || k->flag == 'c' ? c->getParamElementType(1) : nullptr;
  Value *v = k->value < 0 ? nullptr : c->getArgOperand(k->value);
  if (!t || (flagTy && !flagTy->isIntegerTy(8)) || (k->flag == 's' && !ty->isIntegerTy()) ||
      (k->flag == 'o' && ty != t) ||
      // A constant offset is a register unless it fits "I" (0 to 31), and an
      // immediate one counts modulo the operand size, so only 32 or 64 bits.
      (bit ? !v->getType()->isIntegerTy() || !ops.contains(",Ir,") || (bits && bits < 32)
           : !(t->isIntegerTy() || (mn == "xchg" && t->isPointerTy())) ||
                 (v && v->getType() != t) ||
                 (bits && bits != c->getDataLayout().getTypeSizeInBits(t))))
    return nullptr;
  IRBuilder<> b(c);
  auto order = AtomicOrdering::SequentiallyConsistent;
  Value *flag = nullptr;
  if (bit) { // the bit at offset v, signed, from the operand's start
    Value *nr = b.CreateSExtOrTrunc(v, c->getDataLayout().getIntPtrType(c->getContext()));
    Value *byte = b.CreateGEP(b.getInt8Ty(), c->getArgOperand(0), b.CreateAShr(nr, 3));
    Value *mask = b.CreateShl(b.getInt8(1), b.CreateTrunc(b.CreateAnd(nr, 7), b.getInt8Ty()));
    Value *old = mn == "bts" ? b.CreateAtomicRMW(AtomicRMWInst::Or, byte, mask, Align(1), order)
                             : b.CreateAtomicRMW(AtomicRMWInst::And, byte, b.CreateNot(mask),
                                                 Align(1), order);
    flag = b.CreateIsNotNull(b.CreateAnd(old, mask));
  } else {
    bool add = mn == "inc" || mn == "add" || mn == "xadd";
    if (!v)
      v = ConstantInt::get(t, 1);
    auto op = mn == "xchg" ? AtomicRMWInst::Xchg : add ? AtomicRMWInst::Add : AtomicRMWInst::Sub;
    Value *old = b.CreateAtomicRMW(op, c->getArgOperand(0), v, MaybeAlign(), order);
    if (k->flag == 'o')
      return old;
    if (k->flag == 'z')
      flag = b.CreateIsNull(add ? b.CreateAdd(old, v) : b.CreateSub(old, v));
  }
  if (k->flag == 's')
    return b.CreateSExt(flag, ty);
  if (flagTy)
    b.CreateStore(b.CreateZExt(flag, flagTy), c->getArgOperand(1));
  return PoisonValue::get(ty);
}

// Inline asm (issue #19). The idioms of the kernel headers in SV-COMP's LDV
// tasks that are exact in IR become IR: one that runs nothing inline or only
// a fence or pause (a compiler barrier, nothing in one thread) goes when it
// has no register output, or when its one output is tied to an input, which
// the output then is (OPTIMIZER_HIDE_VAR, RELOC_HIDE); ud2 becomes
// llvm.trap, which ends the path as __builtin_trap does, where the program
// expects no return (BUG()), but not where a trap handler may resume after
// it, as newer kernels' WARN() has it; bswap becomes
// llvm.bswap; the kernel's atomics are above, and its cmpxchg becomes a
// cmpxchg. Of its MMIO accessors (asm/io.h), a write is a
// store, and a read, a mov from memory into a register, writes no memory, so
// it loses its "memory" clobber and the walker gives it a nondet value, as a
// device register may hold anything (its operand is still loaded, for the
// checks on the pointer). An asm with alternatives, code the kernel may
// patch in at boot, or in Intel syntax is none of these. The walker handles
// the rest.
static void lowerInlineAsm(Module &m) {
  SmallVector<CallInst *, 8> asms;
  for (Function &f : m)
    for (Instruction &inst : instructions(f))
      if (auto *c = dyn_cast<CallInst>(&inst); c && c->isInlineAsm())
        if (auto *ia = cast<InlineAsm>(c->getCalledOperand());
            ia->getDialect() == InlineAsm::AD_ATT && !ia->getAsmString().contains(".altinstr"))
          asms.push_back(c);
  for (CallInst *c : asms) {
    auto *ia = cast<InlineAsm>(c->getCalledOperand());
    unsigned bits;
    std::string code = inlineInstructions(ia->getAsmString(), bits);
    SmallVector<StringRef, 4> operands;
    SmallVector<std::string, 4> kinds;
    for (StringRef k : split(ia->getConstraintString(), ','))
      if (!k.starts_with("~")) {
        operands.push_back(k);
        kinds.push_back(k.starts_with("=*") ? "=*m"
                        : k.starts_with("*") ? "*m"
                        : k.starts_with("=") ? "=v"
                        : isDigit(k[0])      ? k.str()
                                             : "v");
      }
    std::string ops = join(operands, ","), shape = join(kinds, ",");
    Type *ty = c->getType();
    auto sized = [&](Type *t) { return !bits || bits == m.getDataLayout().getTypeSizeInBits(t); };
    IRBuilder<> b(c);
    Value *r = nullptr;
    if (is_contained({"", "mfence", "lfence", "sfence", "pause", "nop", "rep ; nop"}, code)) {
      if (ty->isVoidTy())
        r = PoisonValue::get(ty);
      else if (ops == "=r,0" && CastInst::isBitOrNoopPointerCastable(
                                    c->getArgOperand(0)->getType(), ty, m.getDataLayout()))
        r = b.CreateBitOrPointerCast(c->getArgOperand(0), ty);
    } else if (code == "ud2" && ty->isVoidTy() && noReturn(c->getNextNode())) {
      b.CreateIntrinsic(Intrinsic::trap, {});
      r = PoisonValue::get(ty);
    } else if (code == "bswap $0" && ops == "=r,0" && ty == c->getArgOperand(0)->getType() &&
               (ty->isIntegerTy(32) || ty->isIntegerTy(64)) && sized(ty)) {
      r = b.CreateUnaryIntrinsic(Intrinsic::bswap, c->getArgOperand(0));
    } else if (code == "mov $0,$1" && shape == "v,*m" &&
               c->getArgOperand(0)->getType() == c->getParamElementType(1) &&
               sized(c->getArgOperand(0)->getType())) {
      b.CreateStore(c->getArgOperand(0), c->getArgOperand(1), /*isVolatile=*/true);
      r = PoisonValue::get(ty);
    } else if (code == "cmpxchg $2,$1" && shape == "=v,=*m,v,0,*m" &&
               StringRef(ops).starts_with("={ax},") && ty == c->getParamElementType(0) &&
               ty->isIntegerTy() && sized(ty) && c->getArgOperand(1)->getType() == ty &&
               c->getArgOperand(2)->getType() == ty) {
      auto order = AtomicOrdering::SequentiallyConsistent;
      r = b.CreateExtractValue(b.CreateAtomicCmpXchg(c->getArgOperand(0), c->getArgOperand(2),
                                                     c->getArgOperand(1), MaybeAlign(), order,
                                                     order),
                               0);
    } else if (code == "mov $1,$0" && shape == "=v,*m" && c->getParamElementType(0)) {
      b.CreateLoad(c->getParamElementType(0), c->getArgOperand(0), /*isVolatile=*/true);
      c->setCalledOperand(InlineAsm::get(ia->getFunctionType(), ia->getAsmString(), ops,
                                         ia->hasSideEffects(), ia->isAlignStack(),
                                         ia->getDialect(), ia->canThrow()));
    } else {
      r = lowerKernelAtomic(c, code, shape, ops, bits);
    }
    if (!r)
      continue;
    if (!ty->isVoidTy())
      c->replaceAllUsesWith(r);
    c->eraseFromParent();
  }
}

// An uninitialised local is one nondet value (plan, section 4.5), as in
// ESBMC. mem2reg would turn each read of it into undef, a new value at
// every use, so each scalar local, and each scalar field of a struct local,
// first gets a call to a bodyless __VERIFIER_nondet_ function, which the
// walker reads as one fresh value. A call, not freeze poison: InstCombine
// may replace that by any constant. Each call is tagged with the variable it
// stands for, so that dropUninitialisedRecords can remove the assignment of
// it that mem2reg would put in the trace.
//
// A local that holds an array, or a variable-length array, stays in memory,
// whose untouched bytes the walker reads as one unknown value already. But
// SROA and InstCombine fold a load from an alloca nothing has written to
// undef, so two reads of one element would differ (issue #9). A call to __vereda_uninitialised, which only writes through its
// argument and does not capture it, right after the alloca keeps them from
// folding; the walker treats it as a no-op.
static const char *UninitialisedMD = "vereda.uninitialised";

static void nondetLocals(Module &m) {
  DenseMap<Type *, FunctionCallee> nondet;
  Function *opaque = cast<Function>(
      m.getOrInsertFunction("__vereda_uninitialised",
                            FunctionType::get(Type::getVoidTy(m.getContext()),
                                              {PointerType::getUnqual(m.getContext())}, false))
          .getCallee());
  opaque->setMemoryEffects(MemoryEffects::argMemOnly(ModRefInfo::Mod));
  opaque->addParamAttr(0, Attribute::getWithCaptureInfo(m.getContext(), CaptureInfo::none()));
  auto markUninitialised = [&](IRBuilder<> &b, AllocaInst *a) { b.CreateCall(opaque, {a}); };
  // The types the encoder gives a value of their own: not long double
  // (x86_fp80) or bfloat, which a local only stores and loads as bits.
  auto scalar = [](Type *ty) {
    return (ty->isIntegerTy() && !ty->isIntegerTy(1)) || ty->isPointerTy() || ty->isHalfTy() ||
           ty->isFloatTy() || ty->isDoubleTy() || ty->isFP128Ty();
  };
  for (Function &f : m) {
    if (f.isDeclaration())
      continue;
    SmallVector<AllocaInst *, 8> locals;
    for (Instruction &inst : instructions(f))
      if (auto *a = dyn_cast<AllocaInst>(&inst))
        locals.push_back(a);
    for (AllocaInst *a : locals) {
      IRBuilder<> b(a->getNextNode());
      b.SetCurrentDebugLocation(DebugLoc()); // no source line: not a store to show
      if (!a->isStaticAlloca()) {
        markUninitialised(b, a);
        continue;
      }
      // Whether the local holds an array with elements; a flexible or
      // zero-length array member holds no bytes to read.
      std::function<bool(Type *)> hasArray = [&](Type *ty) {
        if (auto *st = dyn_cast<StructType>(ty))
          return any_of(st->elements(), hasArray);
        auto *at = dyn_cast<ArrayType>(ty);
        return at && at->getNumElements() > 0;
      };
      if (a->isArrayAllocation() || hasArray(a->getAllocatedType()))
        markUninitialised(b, a);
      TinyPtrVector<DbgVariableRecord *> declares = findDVRDeclares(a);
      MDNode *variable = declares.empty() ? nullptr : declares.front()->getVariable();
      // The scalar leaves of the local, with their index paths.
      std::function<void(Type *, SmallVector<Value *, 4> &)> leaves =
          [&](Type *ty, SmallVector<Value *, 4> &path) {
            if (auto *st = dyn_cast<StructType>(ty)) {
              for (unsigned i = 0; i < st->getNumElements(); ++i) {
                path.push_back(b.getInt32(i));
                leaves(st->getElementType(i), path);
                path.pop_back();
              }
              return;
            }
            if (!scalar(ty))
              return;
            FunctionCallee &fn = nondet[ty];
            if (!fn) {
              std::string name;
              raw_string_ostream(name) << "__VERIFIER_nondet_uninitialised." << *ty;
              fn = m.getOrInsertFunction(name, FunctionType::get(ty, false));
            }
            Value *at = path.size() == 1 ? a : b.CreateInBoundsGEP(a->getAllocatedType(), a, path);
            CallInst *value = b.CreateCall(fn);
            if (variable)
              value->setMetadata(UninitialisedMD, MDNode::get(m.getContext(), {variable}));
            b.CreateStore(value, at);
          };
      SmallVector<Value *, 4> path{b.getInt32(0)};
      leaves(a->getAllocatedType(), path);
    }
  }
}

// Removes the debug records that assign a local its own uninitialised value
// when nothing reads that value (its only users are the stores nondetLocals
// made, into the local itself): the local is written before it is read, and
// ESBMC shows no declaration for it. When the value is read, copied into
// another variable included, its record stays, as ESBMC shows that
// declaration with its value.
static void dropUninitialisedRecords(Module &m) {
  for (Function &f : m)
    for (Instruction &inst : instructions(f))
      for (DbgVariableRecord &rec : make_early_inc_range(filterDbgVars(inst.getDbgRecordRange())))
        if (rec.getNumVariableLocationOps() == 1)
          if (auto *c = dyn_cast_or_null<CallInst>(rec.getVariableLocationOp(0)))
            if (MDNode *md = c->getMetadata(UninitialisedMD))
              if (all_of(c->users(),
                         [&](User *u) {
                           auto *st = dyn_cast<StoreInst>(u);
                           return st && !st->getDebugLoc() &&
                                  isa<AllocaInst>(st->getPointerOperand()->stripInBoundsOffsets());
                         }) &&
                  md->getOperand(0) == rec.getVariable())
                rec.eraseFromParent();
}

static bool isSetjmp(StringRef n) {
  return n == "setjmp" || n == "_setjmp" || n == "__sigsetjmp" || n == "sigsetjmp";
}
static bool isLongjmp(StringRef n) {
  return n == "longjmp" || n == "_longjmp" || n == "siglongjmp" || n == "__longjmp_chk";
}

// setjmp and longjmp become ordinary control flow (#11), as ESBMC's
// lower_longjmp does. Globals hold a jump in flight: pending, the value
// setjmp is to return and a token naming the setjmp's frame and call site.
// A setjmp takes a token from a counter the first time its frame runs it,
// keeps it in a slot of the frame, stores it in its jmp_buf and returns 0
// into a block of its own, its resume block. A longjmp loads the token from
// its jmp_buf, sets the value (1 for 0, C11 7.13.2.1p4) and pending, and
// goes to a dispatch; so does each call that may longjmp when it returns
// with pending set. The dispatch compares the token with the slot of each
// setjmp of its function: a match clears pending and goes back to that
// setjmp's resume block with the value, a loop the walker bounds like any
// other. No match calls __vereda_longjmp_unmatched and returns, for the
// caller to dispatch in turn; in the outermost frame the walker checks it as
// a longjmp to a setjmp no longer active, undefined behaviour by
// 7.13.2.1p2, as ESBMC does.
//
// This runs before mem2reg, which gives each local that is not volatile
// the value it had at the longjmp when the jump arrives. 7.13.2.1p3 makes
// those changed since the setjmp indeterminate; the value they had is the
// one a program compiled at -O0 sees, and sv-benchmarks' longjmp tasks
// expect it (42-poison-reduced_true).
static void lowerLongjmp(Module &m) {
  if (none_of(m, [](Function &f) { return isSetjmp(f.getName()) || isLongjmp(f.getName()); }))
    return;
  lowerCalls(m); // a longjmp through a pointer becomes a direct call
  auto callee = [](Instruction &inst) -> Function * {
    auto *c = dyn_cast<CallInst>(&inst);
    return c ? dyn_cast<Function>(c->getCalledOperand()->stripPointerCasts()) : nullptr;
  };
  // The setjmps and longjmps this pass can lower; any other is left to the
  // walker, which refuses it when reached.
  auto lowerable = [&](Instruction &inst, bool (*is)(StringRef)) {
    Function *f = callee(inst);
    if (!f || !is(f->getName()))
      return false;
    auto &c = cast<CallInst>(inst);
    return c.arg_size() >= 1 + (is == isLongjmp) && c.getArgOperand(0)->getType()->isPointerTy() &&
           (is == isSetjmp ? c.use_empty() || c.getType()->isIntegerTy()
                           : c.getArgOperand(1)->getType()->isIntegerTy());
  };
  // The functions that may longjmp out of a call to them.
  DenseSet<Function *> jumps;
  auto mayJump = [&](Instruction &inst) {
    Function *g = callee(inst);
    return lowerable(inst, isLongjmp) || (g && jumps.contains(g));
  };
  for (bool changed = true; changed;) {
    changed = false;
    for (Function &f : m)
      if (!jumps.contains(&f) && any_of(instructions(f), mayJump)) {
        jumps.insert(&f);
        changed = true;
      }
  }

  LLVMContext &ctx = m.getContext();
  Type *i8 = Type::getInt8Ty(ctx), *i32 = Type::getInt32Ty(ctx), *i64 = Type::getInt64Ty(ctx);
  auto global = [&](Type *ty, StringRef name) {
    return new GlobalVariable(m, ty, false, GlobalValue::InternalLinkage,
                              Constant::getNullValue(ty), name);
  };
  GlobalVariable *pending = global(i8, "__vereda_longjmp_pending"),
                 *value = global(i32, "__vereda_longjmp_value"),
                 *token = global(i64, "__vereda_longjmp_token"),
                 *counter = global(i64, "__vereda_setjmp_count");
  FunctionType *voidFn = FunctionType::get(Type::getVoidTy(ctx), false);
  FunctionCallee unmatched = m.getOrInsertFunction("__vereda_longjmp_unmatched", voidFn),
                 unsupported = m.getOrInsertFunction("__vereda_longjmp_unsupported", voidFn);

  for (Function &f : m) {
    if (f.isDeclaration())
      continue;
    SmallVector<CallInst *, 4> setjmps, points;
    for (Instruction &inst : instructions(f)) {
      if (lowerable(inst, isSetjmp))
        setjmps.push_back(cast<CallInst>(&inst));
      else if (mayJump(inst))
        points.push_back(cast<CallInst>(&inst));
    }

    struct Site {
      AllocaInst *slot;
      BasicBlock *resume;
      PHINode *result;
    };
    SmallVector<Site, 2> sites;
    BasicBlock &entry = f.getEntryBlock();
    // The slots are zeroed in a block of the static allocas alone, which
    // mem2reg still promotes.
    if (!setjmps.empty()) {
      Instruction *first = &*find_if(entry, [](Instruction &i) { return !isa<AllocaInst>(i); });
      for (Instruction &i : make_early_inc_range(make_range(first->getIterator(), entry.end())))
        if (auto *a = dyn_cast<AllocaInst>(&i); a && isa<Constant>(a->getArraySize()))
          a->moveBefore(first->getIterator());
      entry.splitBasicBlock(first);
    }
    DebugLoc loopLoc = setjmps.empty() ? DebugLoc() : setjmps.front()->getDebugLoc();
    for (CallInst *c : setjmps) {
      IRBuilder<> b(entry.getTerminator());
      AllocaInst *slot = b.CreateAlloca(i64);
      b.CreateStore(b.getInt64(0), slot);
      b.SetInsertPoint(c);
      Value *next = b.CreateAdd(b.CreateLoad(i64, counter), b.getInt64(1));
      Value *old = b.CreateLoad(i64, slot);
      Value *n = b.CreateSelect(b.CreateIsNull(old), next, old);
      b.CreateStore(next, counter);
      b.CreateAlignedStore(n, c->getArgOperand(0), Align(1));
      b.CreateStore(n, slot);
      BasicBlock *from = c->getParent(), *resume = from->splitBasicBlock(c, "setjmp.resume");
      from->getTerminator()->setDebugLoc(c->getDebugLoc()); // the jump-back loop's location
      PHINode *result = nullptr;
      if (!c->use_empty()) {
        result = PHINode::Create(c->getType(), 0, "setjmp", resume->begin());
        result->addIncoming(ConstantInt::get(c->getType(), 0), from);
        c->replaceAllUsesWith(result);
      }
      c->eraseFromParent();
      sites.push_back({slot, resume, result});
    }

    // When each resume block dominates every jump, a jump goes straight
    // back to it, making a loop with the resume block as its head. Else all
    // go through restart, between the allocas and the rest of the function,
    // which switches to the resume block: those loops' head is restart,
    // which dominates every block. Mixing the two would let restart skip
    // the head of a direct jump's loop. The loops are irreducible after all
    // when restart enters a loop at a resume block; the walker refuses such
    // a function.
    DominatorTree dt(f);
    bool direct = all_of(points, [&](CallInst *p) {
      return all_of(sites, [&](Site &s) { return dt.dominates(s.resume, p->getParent()); });
    });
    PHINode *target = nullptr; // in restart, the site a jump resumes, from 1
    if (!direct) {
      BasicBlock *restart = entry.splitBasicBlock(entry.getTerminator(), "longjmp.restart");
      BasicBlock *rest = restart->getUniqueSuccessor();
      restart->getTerminator()->eraseFromParent();
      IRBuilder<> b(restart);
      b.SetCurrentDebugLocation(loopLoc);
      target = b.CreatePHI(i32, 0);
      target->addIncoming(b.getInt32(0), &entry);
      Value *resumed = b.CreateLoad(i32, value);
      for (Site &s : sites)
        if (s.result)
          s.result->addIncoming(b.CreateSExtOrTrunc(resumed, s.result->getType()), restart);
      SwitchInst *sw = b.CreateSwitch(target, rest, sites.size());
      for (auto [i, s] : enumerate(sites))
        sw->addCase(b.getInt32(i + 1), s.resume);
    }

    for (CallInst *p : points) {
      BasicBlock *from = p->getParent();
      BasicBlock *dispatch = BasicBlock::Create(ctx, "longjmp.dispatch", &f);
      IRBuilder<> b(p); // the dispatch's checks and calls get p's location
      bool longjmp = lowerable(*p, isLongjmp);
      if (longjmp) {
        Value *v = b.CreateSExtOrTrunc(p->getArgOperand(1), i32);
        b.CreateStore(b.CreateAlignedLoad(i64, p->getArgOperand(0), Align(1)), token);
        b.CreateStore(b.CreateSelect(b.CreateIsNull(v), b.getInt32(1), v), value);
        b.CreateStore(b.getInt8(1), pending);
      }
      // After a longjmp, the block left holds the call itself, now dead.
      BasicBlock *next = from->splitBasicBlock(longjmp ? p : p->getNextNode());
      from->getTerminator()->eraseFromParent();
      b.SetInsertPoint(from);
      if (longjmp)
        b.CreateBr(dispatch);
      else
        b.CreateCondBr(b.CreateIsNotNull(b.CreateLoad(i8, pending)), dispatch, next);
      b.SetInsertPoint(dispatch);
      Value *n = b.CreateLoad(i64, token);
      for (auto [i, s] : enumerate(sites)) {
        Value *slot = b.CreateLoad(i64, s.slot);
        BasicBlock *hit = BasicBlock::Create(ctx, "longjmp.hit", &f),
                   *miss = BasicBlock::Create(ctx, "longjmp.miss", &f);
        b.CreateCondBr(b.CreateAnd(b.CreateIsNotNull(slot), b.CreateICmpEQ(n, slot)), hit, miss);
        b.SetInsertPoint(hit);
        b.CreateStore(b.getInt8(0), pending);
        if (direct) {
          if (s.result)
            s.result->addIncoming(b.CreateSExtOrTrunc(b.CreateLoad(i32, value), s.result->getType()),
                                  hit);
          b.CreateBr(s.resume);
        } else {
          b.CreateBr(target->getParent());
          target->addIncoming(b.getInt32(i + 1), hit);
        }
        b.SetInsertPoint(miss);
      }
      b.CreateCall(unmatched);
      if (f.getReturnType()->isVoidTy())
        b.CreateRetVoid();
      else
        b.CreateRet(PoisonValue::get(f.getReturnType()));
    }

    // Through restart, a jump skips the code before its setjmp, which may
    // define a value used after it (a VLA's address). Such a function only
    // calls __vereda_longjmp_unsupported, which gives UNKNOWN when reached.
    if (target && verifyFunction(f)) {
      DISubprogram *sp = f.getSubprogram();
      f.dropAllReferences();
      f.setSubprogram(sp);
      IRBuilder<> b(BasicBlock::Create(ctx, "", &f));
      b.SetCurrentDebugLocation(loopLoc);
      b.CreateCall(unsupported);
      b.CreateUnreachable();
    }
  }
}

// -fno-finite-loops should have kept these out; any left would let LoopInfo
// and SCEV assume termination.
static bool hasMustProgress(const Module &m) {
  for (const Function &f : m) {
    if (f.hasFnAttribute(Attribute::MustProgress))
      return true;
    for (const Instruction &inst : instructions(f))
      if (MDNode *loopMD = inst.getMetadata(LLVMContext::MD_loop))
        for (const MDOperand &op : loopMD->operands())
          if (auto *node = dyn_cast<MDNode>(op))
            if (node->getNumOperands() > 0)
              if (auto *str = dyn_cast<MDString>(node->getOperand(0)))
                if (str->getString() == "llvm.loop.mustprogress")
                  return true;
  }
  return false;
}

static Error runPipeline(Module &m, StringRef pipeline) {
  LoopAnalysisManager lam;
  FunctionAnalysisManager fam;
  CGSCCAnalysisManager cgam;
  ModuleAnalysisManager mam;
  PassBuilder pb;
  pb.registerModuleAnalyses(mam);
  pb.registerCGSCCAnalyses(cgam);
  pb.registerFunctionAnalyses(fam);
  pb.registerLoopAnalyses(lam);
  pb.crossRegisterProxies(lam, fam, cgam, mam);
  ModulePassManager mpm;
  if (Error e = pb.parsePassPipeline(mpm, pipeline))
    return e;
  mpm.run(m, mam);
  return Error::success();
}

// Links the models a module calls without defining, after its passes, so
// that calls the passes created (bcmp from memcmp) find theirs too. The
// models went through the same passes when built; calls into them whose
// type differs from the definition are converted as lowerCalls does.
static Error linkModels(Module &m, StringRef dir) {
  SmallString<128> path(dir);
  sys::path::append(path, "models-" + std::to_string(m.getDataLayout().getPointerSizeInBits()) +
                              ".bc");
  if (dir.empty() || !sys::fs::exists(path))
    return Error::success();
  SMDiagnostic diag;
  std::unique_ptr<Module> models = parseIRFile(path, diag, m.getContext());
  if (!models)
    return createStringError(inconvertibleErrorCode(), "cannot read " + path);
  models->setTargetTriple(m.getTargetTriple());
  models->setDataLayout(m.getDataLayout());
  if (Linker::linkModules(m, std::move(models), Linker::Flags::LinkOnlyNeeded))
    return createStringError(inconvertibleErrorCode(), "cannot link " + path);
  lowerCalls(m);
  stripUBAssumptions(m);
  return Error::success();
}

Expected<std::unique_ptr<Module>> load(StringRef path, LLVMContext &ctx, StringRef models) {
  SMDiagnostic diag;
  std::unique_ptr<Module> module = parseIRFile(path, diag, ctx);
  if (!module) {
    std::string msg;
    raw_string_ostream os(msg);
    diag.print(nullptr, os, /*ShowColors=*/false);
    return createStringError(inconvertibleErrorCode(), StringRef(msg).rtrim());
  }
  if (hasMustProgress(*module))
    return createStringError(inconvertibleErrorCode(),
                             path + " carries mustprogress; compile with -fno-finite-loops");

  stripUBAssumptions(*module);
  nondetLocals(*module);
  lowerLongjmp(*module);
  if (Error e = runPipeline(*module, "function(mem2reg,sroa)"))
    return std::move(e);
  lowerCalls(*module); // before InstCombine, which folds tests of constants
  libmToIntrinsics(*module);
  lowerInlineAsm(*module);
  // Plain instcombine; the plan wants a "careful" one that folds
  // nothing on undefined behaviour. Revisit when a folded check shows up.
  // no-verify-fixpoint, as LLVM's own pipelines run it: a textual pipeline
  // otherwise aborts when InstCombine needs more than one iteration.
  // inferattrs, last, gives C library declarations the attributes that tell
  // the walker which arguments a bodyless call only reads.
  if (Error e = runPipeline(*module,
                            "function(instcombine<no-verify-fixpoint>,"
                            "scalarizer<load-store;variable-insert-extract>,loop-simplify,lcssa),"
                            "inferattrs"))
    return std::move(e);
  stripUBAssumptions(*module);
  dropUninitialisedRecords(*module);
  if (Error e = linkModels(*module, models))
    return std::move(e);
  return module;
}

} // namespace vereda::ir
