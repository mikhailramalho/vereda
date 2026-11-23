#include "ir/load.h"

#include <llvm/IR/AttributeMask.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Intrinsics.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Operator.h>
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
// (which would fold checks with them) and after (which may infer them back).
static void stripUBAssumptions(Module &m) {
  AttributeMask ubAttrs;
  for (auto k : {Attribute::NoUndef, Attribute::NonNull, Attribute::Dereferenceable,
                 Attribute::DereferenceableOrNull, Attribute::Range, Attribute::OptimizeNone})
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
// may replace that by any constant. Arrays stay in memory, whose initial
// bytes are nondet already. Each call is tagged with the variable it stands
// for, so that dropUninitialisedRecords can remove the assignment of it
// that mem2reg would put in the trace.
static const char *UninitialisedMD = "vereda.uninitialised";

static void nondetLocals(Module &m) {
  DenseMap<Type *, FunctionCallee> nondet;
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
    for (Instruction &inst : f.getEntryBlock())
      if (auto *a = dyn_cast<AllocaInst>(&inst); a && a->isStaticAlloca())
        locals.push_back(a);
    for (AllocaInst *a : locals) {
      IRBuilder<> b(a->getNextNode());
      b.SetCurrentDebugLocation(DebugLoc()); // no source line: not a store to show
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
  if (Error e = runPipeline(*module, "function(mem2reg,sroa)"))
    return std::move(e);
  lowerCalls(*module); // before InstCombine, which folds tests of constants
  libmToIntrinsics(*module);
  // Plain instcombine; the plan wants a "careful" one that folds
  // nothing on undefined behaviour. Revisit when a folded check shows up.
  // no-verify-fixpoint, as LLVM's own pipelines run it: a textual pipeline
  // otherwise aborts when InstCombine needs more than one iteration.
  if (Error e = runPipeline(*module,
                            "function(instcombine<no-verify-fixpoint>,"
                            "scalarizer<load-store;variable-insert-extract>,loop-simplify,lcssa)"))
    return std::move(e);
  stripUBAssumptions(*module);
  dropUninitialisedRecords(*module);
  if (Error e = linkModels(*module, models))
    return std::move(e);
  return module;
}

} // namespace vereda::ir
