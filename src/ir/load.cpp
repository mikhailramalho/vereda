#include "ir/load.h"

#include <llvm/IR/AttributeMask.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Dominators.h>
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
