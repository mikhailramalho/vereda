#include "ir/load.h"
#include "util/assert.h"

#include <llvm/ADT/DepthFirstIterator.h>
#include <llvm/ADT/SetVector.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/Analysis/CycleAnalysis.h>
#include <llvm/Analysis/SimplifyQuery.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/AttributeMask.h>
#include <llvm/IR/Attributes.h>
#include <llvm/IR/DebugInfo.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/GetElementPtrTypeIterator.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/IntrinsicInst.h>
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
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <llvm/Transforms/Utils/FixIrreducible.h>
#include <llvm/Transforms/Utils/LowerSwitch.h>

#include <set>

using namespace llvm;

namespace vereda::ir {

// Keep the check even when mem2reg or InstCombine removes an unused division.
static void checkDivisors(Module &m) {
  for (Function &f : m) {
    SmallVector<Instruction *, 16> divisions;
    for (Instruction &i : instructions(f))
      if (i.getOpcode() == Instruction::SDiv || i.getOpcode() == Instruction::UDiv ||
          i.getOpcode() == Instruction::SRem || i.getOpcode() == Instruction::URem)
        divisions.push_back(&i);
    if (divisions.empty())
      continue;
    DominatorTree dt(f);
    for (Instruction *i : divisions) {
      IRBuilder<> b(i);
      Value *divisor = i->getOperand(1);
      // Clang's sanitizer or a source branch may already exclude zero here.
      if (isKnownNonZero(divisor, SimplifyQuery(m.getDataLayout(), &dt, nullptr, i,
                                               /*UseInstrInfo=*/false, /*CanUseUndef=*/false)))
        continue;
      Value *ok;
      if (auto *vt = dyn_cast<FixedVectorType>(divisor->getType())) {
        ok = b.getTrue();
        for (unsigned lane = 0; lane < vt->getNumElements(); ++lane) {
          Value *element = b.CreateExtractElement(divisor, b.getInt32(lane));
          ok = b.CreateAnd(ok, b.CreateICmpNE(element, Constant::getNullValue(element->getType())));
        }
      } else {
        ok = b.CreateICmpNE(divisor, Constant::getNullValue(divisor->getType()));
        if (ok->getType()->isVectorTy())
          ok = b.CreateAndReduce(ok);
      }
      if (auto *c = dyn_cast<ConstantInt>(ok); c && c->isOne())
        continue;
      FunctionCallee check = m.getOrInsertFunction("__vereda_check_divisor", b.getVoidTy(), b.getInt1Ty());
      b.CreateCall(check, {ok});
    }
  }
}

// Preserve checks even when the result is unused or the operand is constant zero.
// Clang's defined-zero builtins mask the poison result with a select, sometimes
// after a cast (clzg/ctzg) or an add (ffs).
static bool maskedCountZero(Value *v, Value *arg, unsigned &budget) {
  if (!budget || v->use_empty())
    return false;
  --budget;
  for (User *u : v->users()) {
    auto *op = dyn_cast<BinaryOperator>(u);
    if (isa<CastInst>(u) || (op && op->getOpcode() == Instruction::Add)) {
      if (!maskedCountZero(u, arg, budget))
        return false;
      continue;
    }
    auto *sel = dyn_cast<SelectInst>(u);
    auto *cmp = sel ? dyn_cast<ICmpInst>(sel->getCondition()) : nullptr;
    if (!cmp || !cmp->isEquality() || cmp->getOperand(0) != arg ||
        !isa<Constant>(cmp->getOperand(1)) ||
        !cast<Constant>(cmp->getOperand(1))->isNullValue())
      return false;
    bool zeroOnTrue = cmp->getPredicate() == ICmpInst::ICMP_EQ;
    if ((zeroOnTrue ? sel->getFalseValue() : sel->getTrueValue()) != v ||
        (zeroOnTrue ? sel->getTrueValue() : sel->getFalseValue()) == v)
      return false;
  }
  return true;
}

static void checkCountZeros(Module &m) {
  for (Function &f : m) {
    SmallVector<IntrinsicInst *, 8> calls;
    for (Instruction &i : instructions(f))
      if (auto *c = dyn_cast<IntrinsicInst>(&i);
          c && (c->getIntrinsicID() == Intrinsic::ctlz || c->getIntrinsicID() == Intrinsic::cttz) &&
          cast<ConstantInt>(c->getArgOperand(1))->isOne())
        calls.push_back(c);
    if (calls.empty())
      continue;
    DominatorTree dt(f);
    for (IntrinsicInst *c : calls) {
      Value *arg = c->getArgOperand(0);
      if (!arg->getType()->isIntegerTy() && !isa<FixedVectorType>(arg->getType()))
        continue;
      unsigned budget = 32;
      if (maskedCountZero(c, arg, budget))
        continue;
      if (isKnownNonZero(arg, SimplifyQuery(m.getDataLayout(), &dt, nullptr, c,
                                           /*UseInstrInfo=*/false, /*CanUseUndef=*/false)))
        continue;
      IRBuilder<> b(c);
      b.SetCurrentDebugLocation(c->getDebugLoc());
      Value *ok = b.CreateICmpNE(arg, Constant::getNullValue(arg->getType()));
      if (auto *vt = dyn_cast<FixedVectorType>(arg->getType())) {
        Value *all = b.getTrue();
        for (unsigned lane = 0; lane < vt->getNumElements(); ++lane)
          all = b.CreateAnd(all, b.CreateExtractElement(ok, b.getInt32(lane)));
        ok = all;
      }
      FunctionCallee check = m.getOrInsertFunction("__vereda_check_count_zero",
                                                   b.getVoidTy(), b.getInt1Ty());
      b.CreateCall(check, {ok});
      // The check now owns the precondition; do not let LLVM exploit poison.
      c->setArgOperand(1, b.getFalse());
    }
  }
}

// The type a pointer to an alloca, a global or a GEP points to, else null.
static Type *pointee(const Value *p) {
  if (auto *a = dyn_cast<AllocaInst>(p))
    return a->getAllocatedType();
  if (auto *g = dyn_cast<GlobalVariable>(p))
    return g->getValueType();
  if (auto *g = dyn_cast<GEPOperator>(p))
    return g->getResultElementType();
  return nullptr;
}

// Clang gives an access through a cast of a declared object's address the
// alignment it proves from the declaration and offsets, not the type's
// (EmitPointerWithAlignment in its CGExpr.cpp): *(int *)(buf + 1) loads with
// align 1, and *(struct S *)(buf + o) copies with a memcpy of align 1, which
// -fsanitize=alignment misses too. In clang's unoptimised IR such a pointer
// is a chain of array or pointer-arithmetic GEPs from an alloca or a global.
// Clang's own accesses below the type's alignment are left out: a GEP into a
// struct field (packed members, argument coercions), a byte offset from a
// struct (bitfields of a packed struct) and a GEP to a struct (a member of a
// packed union, which has no field GEP).
static bool castFromDeclaration(const Value *p) {
  auto *outer = dyn_cast<GEPOperator>(p);
  if (!outer || isa<StructType>(outer->getResultElementType()) ||
      isa_and_present<StructType>(pointee(outer->getPointerOperand())))
    return false;
  while (auto *g = dyn_cast<GEPOperator>(p)) {
    for (gep_type_iterator i = gep_type_begin(g), e = gep_type_end(g); i != e; ++i)
      if (i.isStruct())
        return false;
    p = g->getPointerOperand();
  }
  return isa<AllocaInst>(p) || isa<GlobalVariable>(p);
}

static bool compiledByClang(const Module &m) {
  if (const NamedMDNode *ident = m.getNamedMetadata("llvm.ident"))
    for (const MDNode *n : ident->operands())
      if (auto *s = n->getNumOperands() ? dyn_cast_or_null<MDString>(n->getOperand(0)) : nullptr;
          s && s->getString().contains("clang"))
        return true;
  return false;
}

// Instrument typed memory accesses (load, store, atomicrmw, cmpxchg) with their
// source-level alignment before SROA/InstCombine can eliminate misaligned
// accesses through pointers to packed members or raise alignment from UB
// assumptions. Direct accesses to allocas whose own alignment already satisfies
// the access alignment are skipped so mem2reg still promotes aligned locals;
// parameter 0 is marked non-capturing so it does not expose objects or block
// array ownership. fromClang: the module is clang's unoptimised IR, where a
// load, store or aggregate copy through castFromDeclaration needs the
// alignment clang dropped.
static void checkAlignments(Module &m, bool fromClang) {
  const DataLayout &dl = m.getDataLayout();
  Function *check = nullptr;
  auto getCheck = [&]() -> Function * {
    if (!check) {
      LLVMContext &ctx = m.getContext();
      check = cast<Function>(
          m.getOrInsertFunction("__vereda_check_alignment",
                                FunctionType::get(Type::getVoidTy(ctx),
                                                  {PointerType::getUnqual(ctx), Type::getInt64Ty(ctx)},
                                                  false))
              .getCallee());
      check->setMemoryEffects(MemoryEffects::none());
      check->addParamAttr(0, Attribute::getWithCaptureInfo(ctx, CaptureInfo::none()));
    }
    return check;
  };
  for (Function &f : m) {
    if (f.isDeclaration())
      continue;
    SmallVector<std::tuple<Instruction *, Value *, uint64_t>, 16> accesses;
    for (Instruction &i : instructions(f)) {
      Value *ptr = nullptr;
      uint64_t align = 1;
      if (auto *l = dyn_cast<LoadInst>(&i)) {
        ptr = l->getPointerOperand();
        align = l->getAlign().value();
      } else if (auto *s = dyn_cast<StoreInst>(&i)) {
        ptr = s->getPointerOperand();
        align = s->getAlign().value();
      } else if (auto *a = dyn_cast<AtomicRMWInst>(&i)) {
        ptr = a->getPointerOperand();
        align = a->getAlign().value();
      } else if (auto *c = dyn_cast<AtomicCmpXchgInst>(&i)) {
        ptr = c->getPointerOperand();
        align = c->getAlign().value();
      } else if (auto *c = dyn_cast<MemTransferInst>(&i); c && fromClang) {
        // Only clang's aggregate copies are intrinsics: the front end makes a
        // user's memcpy a library call. Such a copy needs the aggregate's
        // alignment, as the other side has it: its type's, unless clang gave
        // it less (a packed aggregate whose layout is natural keeps its type's).
        auto *length = dyn_cast<ConstantInt>(c->getLength());
        for (auto [side, sideAlign, other, otherAlign] :
             {std::tuple(c->getRawDest(), c->getDestAlign(), c->getRawSource(), c->getSourceAlign()),
              std::tuple(c->getRawSource(), c->getSourceAlign(), c->getRawDest(), c->getDestAlign())}) {
          Type *t = pointee(other);
          if (!length || !t || !t->isAggregateType() || !t->isSized() ||
              dl.getTypeAllocSize(t) != length->getZExtValue())
            continue;
          Align need = std::min(dl.getABITypeAlign(t), otherAlign.valueOrOne());
          if (need > sideAlign.valueOrOne() && side->getType()->getPointerAddressSpace() == 0 &&
              castFromDeclaration(side))
            accesses.emplace_back(&i, side, need.value());
        }
      }
      if (fromClang && (isa<LoadInst>(i) || isa<StoreInst>(i)) && castFromDeclaration(ptr))
        align = std::max(align, dl.getABITypeAlign(getLoadStoreType(&i)).value());
      if (!ptr || align <= 1 || ptr->getType()->getPointerAddressSpace() != 0)
        continue;
      // Direct access to an alloca whose own alignment already satisfies align.
      if (auto *a = dyn_cast<AllocaInst>(ptr); a && a->getAlign().value() >= align)
        continue;
      accesses.emplace_back(&i, ptr, align);
    }
    for (auto &[inst, ptr, align] : accesses) {
      IRBuilder<> b(inst);
      b.SetCurrentDebugLocation(inst->getDebugLoc());
      b.CreateCall(getCheck(), {ptr, b.getInt64(align)});
    }
  }
}

// True only when the immediate predecessor branches on a Clang ubsan
// float-cast-overflow guard whose fcmp bounds provably cover this specific
// cast's operand and required [lo, hi) range (preventing a wider guard such as
// double->i32 from suppressing instrumentation on a narrower cast like double->i8).
static bool hasUbsanFloatCastGuard(const CastInst &cast, const APFloat &loVal,
                                   CmpInst::Predicate loPred, const APFloat &hiVal) {
  const BasicBlock *bb = cast.getParent();
  const BasicBlock *pred = bb->getSinglePredecessor();
  if (!pred)
    return false;
  auto *br = dyn_cast<CondBrInst>(pred->getTerminator());
  if (!br || br->getSuccessor(0) != bb)
    return false;
  bool callsHandler = false;
  for (const Instruction &i : *br->getSuccessor(1))
    if (auto *call = dyn_cast<CallBase>(&i))
      if (const Function *f = call->getCalledFunction())
        if (f->getName() == "__ubsan_handle_float_cast_overflow_abort") {
          callsHandler = true;
          break;
        }
  if (!callsHandler)
    return false;
  auto *cond = dyn_cast<BinaryOperator>(br->getCondition());
  if (!cond || cond->getOpcode() != Instruction::And)
    return false;
  const Value *src = cast.getOperand(0);
  bool loCovered = false, hiCovered = false;
  for (unsigned idx = 0; idx < 2; ++idx) {
    auto *fc = dyn_cast<FCmpInst>(cond->getOperand(idx));
    if (!fc || fc->getOperand(0) != src)
      continue;
    auto *cf = dyn_cast<ConstantFP>(fc->getOperand(1));
    if (!cf)
      continue;
    APFloat c = cf->getValueAPF();
    if (&c.getSemantics() != &loVal.getSemantics() || c.isNaN())
      continue;
    CmpInst::Predicate p = fc->getPredicate();
    if (p == CmpInst::FCMP_OLT) {
      if (c.compare(hiVal) != APFloat::cmpGreaterThan)
        hiCovered = true;
    } else if (p == CmpInst::FCMP_OLE) {
      if (c.compare(hiVal) == APFloat::cmpLessThan)
        hiCovered = true;
    } else if (p == CmpInst::FCMP_OGT) {
      if (loPred == CmpInst::FCMP_OGT) {
        if (c.compare(loVal) != APFloat::cmpLessThan)
          loCovered = true;
      } else if (loPred == CmpInst::FCMP_OGE) {
        // x > c implies x >= loVal iff nextUp(c) >= loVal (e.g. Clang's
        // fcmp ogt float %x, nextDown(-2^31) for float -> i32).
        APFloat next = c;
        next.next(/*nextDown=*/false);
        if (next.compare(loVal) != APFloat::cmpLessThan)
          loCovered = true;
      }
    } else if (p == CmpInst::FCMP_OGE) {
      if (loPred == CmpInst::FCMP_OGT) {
        if (c.compare(loVal) == APFloat::cmpGreaterThan)
          loCovered = true;
      } else if (loPred == CmpInst::FCMP_OGE) {
        if (c.compare(loVal) != APFloat::cmpLessThan)
          loCovered = true;
      }
    }
  }
  return loCovered && hiCovered;
}

// Gives a check the comment ESBMC gives the same check (ir::checkComment).
static void nameCheck(Instruction &check, const Twine &comment) {
  LLVMContext &ctx = check.getContext();
  check.setMetadata("vereda.check", MDNode::get(ctx, MDString::get(ctx, comment.str())));
}

// ESBMC names the destination by its internal type: signedbv or unsignedbv.
static std::string floatCastComment(const Instruction &cast) {
  return std::string("floating-point conversion out of range of ") +
         (cast.getOpcode() == Instruction::FPToSI ? "signedbv" : "unsignedbv") + " on typecast";
}

// The operation a clang sanitizer check at call guards: the first
// instrumented one on from the branch into the check's block, past the
// checks clang chains in front of it (whose blocks end in unreachable).
static Instruction *guardedOperation(CallBase &call) {
  BasicBlock *check = call.getParent();
  BasicBlock *pred = check->getSinglePredecessor();
  auto *br = pred ? dyn_cast<CondBrInst>(pred->getTerminator()) : nullptr;
  BasicBlock *next = br ? br->getSuccessor(br->getSuccessor(0) == check) : nullptr;
  for (unsigned n = 0; next && n < 8; ++n) {
    for (Instruction &i : *next) {
      if (i.hasMetadata(LLVMContext::MD_nosanitize))
        continue;
      switch (i.getOpcode()) {
      case Instruction::SDiv: case Instruction::SRem: case Instruction::UDiv:
      case Instruction::URem: case Instruction::Shl: case Instruction::LShr:
      case Instruction::AShr: case Instruction::FPToSI: case Instruction::FPToUI:
        return &i;
      }
    }
    br = dyn_cast<CondBrInst>(next->getTerminator());
    if (!br)
      return nullptr;
    BasicBlock *a = br->getSuccessor(0), *b = br->getSuccessor(1);
    next = isa<UnreachableInst>(a->getTerminator())   ? b
           : isa<UnreachableInst>(b->getTerminator()) ? a
                                                      : nullptr;
  }
  return nullptr;
}

// Records the clang sanitizers whose traps the module holds, as -fsanitize=
// names them, in the named metadata vereda.sanitized, for the driver's
// [INFO] Checks line. It runs on the IR as loaded, before the passes reshape
// it: an overflow trap is signed or unsigned by the overflow intrinsics in
// the blocks that branch to it (with optimisation, clang shares one trap
// block between the checks of a function).
static void recordSanitizers(Module &m) {
  std::set<std::string> names;
  for (Function &f : m)
    for (Instruction &i : instructions(f)) {
      auto *c = dyn_cast<CallBase>(&i);
      Function *callee = c ? c->getCalledFunction() : nullptr;
      if (!callee || callee->getName() != "llvm.ubsantrap")
        continue;
      switch (cast<ConstantInt>(c->getArgOperand(0))->getZExtValue()) {
      case 20: // ShiftOutOfBounds
        names.insert("shift");
        break;
      case 3: // DivremOverflow
        names.insert("signed-integer-overflow");
        break;
      case 0: case 12: case 13: case 21: // Add, Mul, Negate and SubOverflow
        for (BasicBlock *pred : predecessors(c->getParent()))
          for (Instruction &p : *pred)
            if (auto *ii = dyn_cast<IntrinsicInst>(&p))
              switch (ii->getIntrinsicID()) {
              case Intrinsic::sadd_with_overflow:
              case Intrinsic::ssub_with_overflow:
              case Intrinsic::smul_with_overflow:
                names.insert("signed-integer-overflow");
                break;
              case Intrinsic::uadd_with_overflow:
              case Intrinsic::usub_with_overflow:
              case Intrinsic::umul_with_overflow:
                names.insert("unsigned-integer-overflow");
                break;
              default:
                break;
              }
        break;
      }
    }
  if (names.empty())
    return;
  NamedMDNode *md = m.getOrInsertNamedMetadata("vereda.sanitized");
  for (const std::string &name : names)
    md->addOperand(MDNode::get(m.getContext(), MDString::get(m.getContext(), name)));
}

// Names clang's division-overflow, shift and float-cast checks as ESBMC
// does, by the operation each guards, which InstCombine may move or merge
// later. Clang checks a signed shl's exponent and base in one branch; ESBMC
// reports a non-negative base that loses bits as an arithmetic overflow
// and the rest as undefined behaviour, so its trap is split in two.
static void nameSanitizerChecks(Module &m) {
  SmallVector<CallBase *, 8> calls;
  for (Function &f : m)
    for (Instruction &i : instructions(f))
      if (auto *c = dyn_cast<CallBase>(&i))
        if (Function *callee = c->getCalledFunction())
          if (callee->getName() == "llvm.ubsantrap" ||
              callee->getName() == "__ubsan_handle_divrem_overflow_abort" ||
              callee->getName() == "__ubsan_handle_float_cast_overflow_abort")
            calls.push_back(c);
  for (CallBase *c : calls) {
    Instruction *op = guardedOperation(*c);
    if (!op)
      continue;
    StringRef callee = c->getCalledFunction()->getName();
    auto *code = callee == "llvm.ubsantrap" ? cast<ConstantInt>(c->getArgOperand(0)) : nullptr;
    switch (op->getOpcode()) {
    case Instruction::SDiv:
    case Instruction::SRem:
      if (callee == "__ubsan_handle_divrem_overflow_abort" ||
          (code && code->equalsInt(3))) // DivremOverflow
        nameCheck(*c, std::string("arithmetic overflow on ") +
                          (op->getOpcode() == Instruction::SDiv ? "div" : "modulus"));
      break;
    case Instruction::Shl:
    case Instruction::LShr:
    case Instruction::AShr: {
      if (!code || !code->equalsInt(20)) // ShiftOutOfBounds
        break;
      nameCheck(*c, Twine("undefined behavior on shift operation ") + op->getOpcodeName());
      // Split only clang's signed shl shape: the predecessor branches on
      // exponent-ok and a phi of the base check, straight into the shl's
      // block. The base, defined above that block, then dominates the
      // predecessor and so the check's block.
      BasicBlock *check = c->getParent();
      BasicBlock *pred = check->getSinglePredecessor();
      auto *br = cast<CondBrInst>(pred->getTerminator());
      auto *both = dyn_cast<BinaryOperator>(br->getCondition());
      Value *base = op->getOperand(0);
      auto *baseInst = dyn_cast<Instruction>(base);
      if (op->getOpcode() != Instruction::Shl || !both || both->getOpcode() != Instruction::And ||
          isa<PHINode>(both->getOperand(0)) == isa<PHINode>(both->getOperand(1)) ||
          op->getParent()->getSinglePredecessor() != pred ||
          (baseInst && baseInst->getParent() == op->getParent()) || c != &check->front())
        break;
      Value *exponentOk = isa<PHINode>(both->getOperand(0)) ? both->getOperand(1)
                                                             : both->getOperand(0);
      LLVMContext &ctx = m.getContext();
      Function *f = check->getParent();
      BasicBlock *overflow = BasicBlock::Create(ctx, "shl.overflow", f);
      BasicBlock *undefined = BasicBlock::Create(ctx, "shl.undefined", f);
      for (BasicBlock *to : {overflow, undefined}) {
        Instruction *copy = c->clone();
        copy->insertInto(to, to->end());
        new UnreachableInst(ctx, to);
        if (to == overflow)
          nameCheck(*copy, "arithmetic overflow on shl");
      }
      check->getTerminator()->eraseFromParent();
      c->eraseFromParent();
      // Overflow as the second successor walks first, so a run that stops at
      // the first failure reports it before the undefined shift, as ESBMC.
      IRBuilder<> b(check);
      b.SetCurrentDebugLocation(op->getDebugLoc());
      Value *negative = b.CreateICmpSLT(base, Constant::getNullValue(base->getType()));
      b.CreateCondBr(b.CreateOr(b.CreateNot(exponentOk), negative), undefined, overflow);
      break;
    }
    case Instruction::FPToSI:
    case Instruction::FPToUI:
      if (callee == "__ubsan_handle_float_cast_overflow_abort")
        nameCheck(*c, floatCastComment(*op));
      break;
    }
  }
}

// Instrument raw fptosi/fptoui instructions with exact C11 6.3.1.4p1 range
// checks (open bounds on the truncated value, rejecting NaN and infinities)
// without rounded floating-point limit constants.
static void checkFloatCasts(Module &m) {
  for (Function &f : m) {
    SmallVector<CastInst *, 8> casts;
    for (Instruction &i : instructions(f))
      if (auto *c = dyn_cast<CastInst>(&i);
          c && (c->getOpcode() == Instruction::FPToSI || c->getOpcode() == Instruction::FPToUI))
        casts.push_back(c);
    for (CastInst *c : casts) {
      Value *src = c->getOperand(0);
      Type *srcTy = src->getType(), *dstTy = c->getType();
      if (isa<ScalableVectorType>(srcTy) || isa<ScalableVectorType>(dstTy))
        continue;
      Type *fpScalar = srcTy->getScalarType();
      Type *intScalar = dstTy->getScalarType();
      if (!fpScalar->isFloatingPointTy() || fpScalar->isPPC_FP128Ty() || !intScalar->isIntegerTy())
        continue;
      const fltSemantics &sem = fpScalar->getFltSemantics();
      unsigned width = intScalar->getIntegerBitWidth();
      unsigned prec = APFloat::semanticsPrecision(sem);
      int maxExp = APFloat::semanticsMaxExponent(sem);
      bool isSigned = c->getOpcode() == Instruction::FPToSI;
      auto splat = [&](const APFloat &val) -> Constant * {
        Constant *sc = ConstantFP::get(m.getContext(), val);
        if (auto *vt = dyn_cast<FixedVectorType>(srcTy))
          return ConstantVector::getSplat(vt->getElementCount(), sc);
        return sc;
      };
      // Upper bound: trunc(x) <= 2^E - 1  <=>  x < 2^E (exact power of two, or +inf).
      unsigned hiExp = isSigned ? width - 1 : width;
      APFloat hiVal = hiExp <= static_cast<unsigned>(maxExp)
                          ? scalbn(APFloat(sem, 1), static_cast<int>(hiExp),
                                   APFloat::rmNearestTiesToEven)
                          : APFloat::getInf(sem, false);
      // Lower bound: unsigned is x > -1.0; signed is x > -(2^(W-1) + 1) when
      // exact in sem (W <= prec), else x >= -2^(W-1) (no float lies between
      // -2^(W-1) and -2^(W-1) - 1 when ULP >= 2), or x > -inf if W-1 > maxExp.
      APFloat loVal(sem);
      CmpInst::Predicate loPred = CmpInst::FCMP_OGT;
      if (!isSigned) {
        loVal = APFloat(sem, 1);
        loVal.changeSign();
      } else if (width - 1 > static_cast<unsigned>(maxExp)) {
        loVal = APFloat::getInf(sem, true);
      } else if (width <= prec) {
        APInt mag = APInt::getOneBitSet(width + 1, width - 1) + 1;
        loVal.convertFromAPInt(mag, /*IsSigned=*/false, APFloat::rmNearestTiesToEven);
        loVal.changeSign();
      } else {
        loVal = scalbn(APFloat(sem, 1), static_cast<int>(width - 1),
                       APFloat::rmNearestTiesToEven);
        loVal.changeSign();
        loPred = CmpInst::FCMP_OGE;
      }
      if (!isa<FixedVectorType>(srcTy) && hasUbsanFloatCastGuard(*c, loVal, loPred, hiVal))
        continue;
      IRBuilder<> b(c);
      b.SetCurrentDebugLocation(c->getDebugLoc());
      Value *loOk = b.CreateFCmp(loPred, src, splat(loVal));
      Value *hiOk = b.CreateFCmpOLT(src, splat(hiVal));
      Value *ok = b.CreateAnd(loOk, hiOk);
      if (auto *vt = dyn_cast<FixedVectorType>(srcTy)) {
        Value *all = b.getTrue();
        for (unsigned lane = 0; lane < vt->getNumElements(); ++lane)
          all = b.CreateAnd(all, b.CreateExtractElement(ok, b.getInt32(lane)));
        ok = all;
      }
      FunctionCallee check = m.getOrInsertFunction("__vereda_check_float_cast_overflow",
                                                   b.getVoidTy(), b.getInt1Ty());
      nameCheck(*b.CreateCall(check, {ok}), floatCastComment(*c));
    }
  }
}

static void dropSafeChecks(Module &m, StringRef name) {
  Function *check = m.getFunction(name);
  if (!check)
    return;
  for (User *user : make_early_inc_range(check->users()))
    if (auto *call = dyn_cast<CallInst>(user))
      if (auto *ok = dyn_cast<ConstantInt>(call->getArgOperand(0)); ok && ok->isOne())
        call->eraseFromParent();
  if (check->use_empty())
    check->eraseFromParent();
}

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
  if (calls.empty())
    return;
  SmallVector<Function *, 16> addressTaken;
  bool collectedAddressTaken = false;
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
    if (!collectedAddressTaken) {
      for (Function &f : m)
        if (f.hasAddressTaken(nullptr, false, true, false, false, /*IgnoreCastedDirectCall=*/true))
          addressTaken.push_back(&f);
      collectedAddressTaken = true;
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
    for (Function *f : addressTaken) {
      if (!compatible(*c, *f, dl))
        continue;
      BasicBlock *call = BasicBlock::Create(ctx, "call." + f->getName(), head->getParent(), tail);
      BasicBlock *next = BasicBlock::Create(ctx, "", head->getParent(), tail);
      b.CreateCondBr(b.CreateICmpEQ(c->getCalledOperand(), f), call, next);
      branch(*f, call);
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
  auto *br = dyn_cast<UncondBrInst>(next);
  if (!br)
    return false;
  auto *loop = dyn_cast<UncondBrInst>(&br->getSuccessor()->front());
  return loop && loop->getSuccessor() == loop->getParent();
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
// it that mem2reg would put in the trace. A scalar local never read before
// it is written gets none (writtenBeforeRead).
//
// A local that holds an array, or a variable-length array, stays in memory,
// whose untouched bytes the walker reads as one unknown value already. But
// SROA and InstCombine fold a load from an alloca nothing has written to
// undef, so two reads of one element would differ (issue #9). A call to __vereda_uninitialised, which only writes through its
// argument and does not capture it, right after the alloca keeps them from
// folding; the walker treats it as a no-op.
static const char *UninitialisedMD = "vereda.uninitialised";

// Whether scalar local a is only stored to and loaded, each load after a
// store on every path to it: then no read sees it uninitialised and
// nondetLocals leaves it alone. Its nondet value would be dead, and would
// cost a store in the entry block, from which mem2reg walks the whole
// dominator tree, once per local; on btor2c's tasks, tens of thousands of
// locals in one main of thousands of blocks, that was most of IR
// preparation.
static bool writtenBeforeRead(AllocaInst *a, const DominatorTree &dt) {
  Type *ty = a->getAllocatedType();
  SmallVector<StoreInst *, 2> stores;
  SmallVector<LoadInst *, 4> loads;
  for (User *u : a->users()) {
    if (auto *st = dyn_cast<StoreInst>(u); st && st->getPointerOperand() == a &&
                                           st->isSimple() && st->getValueOperand()->getType() == ty)
      stores.push_back(st);
    else if (auto *ld = dyn_cast<LoadInst>(u); ld && ld->isSimple() && ld->getType() == ty)
      loads.push_back(ld);
    else
      return false;
  }
  return all_of(loads, [&](LoadInst *ld) {
    return any_of(stores, [&](StoreInst *st) { return dt.dominates(st, ld); });
  });
}

static void nondetLocals(Module &m, bool uninitCheck) {
  DenseMap<Type *, FunctionCallee> nondet;
  Function *opaque = cast<Function>(
      m.getOrInsertFunction("__vereda_uninitialised",
                            FunctionType::get(Type::getVoidTy(m.getContext()),
                                              {PointerType::getUnqual(m.getContext())}, false))
          .getCallee());
  opaque->setMemoryEffects(MemoryEffects::argMemOnly(ModRefInfo::Mod));
  opaque->addParamAttr(0, Attribute::getWithCaptureInfo(m.getContext(), CaptureInfo::none()));
  auto markUninitialised = [&](IRBuilder<> &b, AllocaInst *a) { b.CreateCall(opaque, {a}); };
  // The types the encoder gives a value of their own: not bfloat or
  // ppc_fp128, which a local only stores and loads as bits.
  auto scalar = [](Type *ty) {
    return (ty->isIntegerTy() && !ty->isIntegerTy(1)) || ty->isPointerTy() || ty->isHalfTy() ||
           ty->isFloatTy() || ty->isDoubleTy() || ty->isX86_FP80Ty() || ty->isFP128Ty();
  };
  for (Function &f : m) {
    if (f.isDeclaration())
      continue;
    // Decided before anything is inserted, which would make each dominance
    // query within the entry block renumber it.
    DominatorTree dt(f);
    SmallVector<AllocaInst *, 8> locals;
    for (Instruction &inst : instructions(f))
      if (auto *a = dyn_cast<AllocaInst>(&inst))
        if (!a->isStaticAlloca() || a->isArrayAllocation() || !scalar(a->getAllocatedType()) ||
            !writtenBeforeRead(a, dt))
          locals.push_back(a);
    for (AllocaInst *a : locals) {
      IRBuilder<> b(a->getNextNode());
      b.SetCurrentDebugLocation(DebugLoc()); // no source line: not a store to show
      if (uninitCheck || !a->isStaticAlloca()) {
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
    // when restart enters a loop at a resume block; fixIrreducible makes
    // them loops again.
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

// Irreducible control flow, a goto into a loop as the reducer-made SV-COMP
// tasks have (#176), becomes natural loops: FixIrreducible routes every way
// into an irreducible cycle, and every edge back to the block it takes as
// the cycle's header, through a new block, irr.guard, that branches to the
// entry taken. The walker unrolls the cycle as a loop headed there, once
// per pass through irr.guard: ESBMC's count of the backward goto, or one
// more on some paths when the pass takes another block than the goto's
// target as the cycle's header. Its branches carry no location, so no
// branching waypoint or trace step comes from them. The loop's location,
// unless its latches already agree on one, is that of a jump back to
// irr.guard; it goes in loop metadata, which loop-simplify also gives the
// preheader it makes. The pass takes only branches, so a switch of such a
// function becomes branches first; one with another terminator
// (indirectbr) is left for the walker to refuse.
static void fixIrreducible(Module &m) {
  FunctionAnalysisManager fam;
  PassBuilder().registerFunctionAnalyses(fam);
  auto reducible = [](Cycle *top) {
    return all_of(depth_first(top), [](Cycle *c) { return c->isReducible(); });
  };
  for (Function &f : m) {
    if (f.isDeclaration() || all_of(fam.getResult<CycleAnalysis>(f).toplevel_cycles(), reducible))
      continue;
    fam.invalidate(f, LowerSwitchPass().run(f, fam));
    if (!hasOnlySimpleTerminator(f))
      continue;
    fam.invalidate(f, FixIrreduciblePass().run(f, fam));
    for (Loop *l : fam.getResult<LoopAnalysis>(f).getLoopsInPreorder()) {
      SmallVector<BasicBlock *, 4> latches;
      l->getLoopLatches(latches);
      auto back =
          find_if(latches, [](BasicBlock *b) { return bool(b->getTerminator()->getDebugLoc()); });
      if (!l->getHeader()->getName().starts_with("irr.guard") || l->getLoopID() ||
          back == latches.end())
        continue;
      MDNode *id = MDNode::getDistinct(
          m.getContext(), {nullptr, (*back)->getTerminator()->getDebugLoc().get()});
      id->replaceOperandWith(0, id);
      for (BasicBlock *b : latches)
        b->getTerminator()->setMetadata(LLVMContext::MD_loop, id);
    }
  }
}

// Destination argument index of a C library call that InstCombine's
// LibCallSimplifier may lower into a write, or -1 otherwise. Only destination
// arguments are unconstanted so source and format string literals stay constant
// for InstCombine's normal simplifications (e.g. stpcpy, sprintf).
static int libCallWriteArg(const Function *f) {
  if (!f || !f->isDeclaration())
    return -1;
  StringRef name = f->getName();
  if (name == "bcopy")
    return 1;
  if (name == "strcpy" || name == "strncpy" || name == "stpcpy" || name == "stpncpy" ||
      name == "strcat" || name == "strncat" || name == "sprintf" || name == "snprintf" ||
      name == "vsprintf" || name == "vsnprintf" || name == "bzero" || name == "memccpy" ||
      name == "mempcpy" || name == "strlcpy" || name == "strlcat" || name == "wcscpy" ||
      name == "wcsncpy" || name == "wcscat" || name == "wcsncat" || name == "gets" ||
      name == "fgets" || name == "fread" || name == "memcpy" || name == "memmove" ||
      name == "memset" || name == "memset_pattern16" || name == "__strcpy_chk" ||
      name == "__strncpy_chk" || name == "__stpcpy_chk" || name == "__stpncpy_chk" ||
      name == "__strcat_chk" || name == "__strncat_chk" || name == "__sprintf_chk" ||
      name == "__snprintf_chk" || name == "__memcpy_chk" || name == "__memmove_chk" ||
      name == "__memset_chk" || name == "__mempcpy_chk")
    return 0;
  return -1;
}

// InstCombine deletes a write into a constant global as undefined
// behaviour, which the walker checks (a write into a string literal or an
// object defined const). This makes the constant globals a store, memory
// intrinsic, or writing library call may write writable, and returns them for
// the caller to make constant again once InstCombine has run.
static SmallVector<GlobalVariable *, 4> unconstantWritten(Module &m) {
  SmallSetVector<GlobalVariable *, 4> written;
  auto mark = [&](const Value *ptr) {
    SmallVector<const Value *, 4> objects;
    getUnderlyingObjects(ptr, objects, nullptr, 0);
    for (const Value *o : objects)
      if (auto *g = dyn_cast<GlobalVariable>(o); g && g->isConstant())
        written.insert(const_cast<GlobalVariable *>(g));
  };
  for (Function &f : m)
    for (Instruction &i : instructions(f)) {
      if (auto *s = dyn_cast<StoreInst>(&i)) {
        mark(s->getPointerOperand());
      } else if (auto *mi = dyn_cast<MemIntrinsic>(&i)) {
        mark(mi->getRawDest());
      } else if (auto *cb = dyn_cast<CallBase>(&i)) {
        int dst = libCallWriteArg(cb->getCalledFunction());
        if (dst >= 0 && static_cast<unsigned>(dst) < cb->arg_size() &&
            cb->getArgOperand(dst)->getType()->isPointerTy())
          mark(cb->getArgOperand(dst));
      }
    }
  for (GlobalVariable *g : written)
    g->setConstant(false);
  return written.takeVector();
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
static Error linkModels(Module &m, StringRef dir, bool divByZeroCheck, bool countZeroCheck,
                        bool floatCastOverflowCheck, bool alignmentCheck) {
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
  if (divByZeroCheck)
    checkDivisors(*models);
  if (countZeroCheck)
    checkCountZeros(*models);
  if (floatCastOverflowCheck)
    checkFloatCasts(*models);
  if (alignmentCheck)
    checkAlignments(*models, false);
  if (Linker::linkModules(m, std::move(models), Linker::Flags::LinkOnlyNeeded))
    return createStringError(inconvertibleErrorCode(), "cannot link " + path);
  lowerCalls(m);
  stripUBAssumptions(m);
  return Error::success();
}

// Rust's C main passes the source main to std::rt::lang_start. Verify that
// function directly rather than the unavailable runtime's startup machinery.
static void lowerRustMain(Module &m) {
  Function *main = m.getFunction("main");
  if (!main)
    return;
  for (BasicBlock &bb : *main)
    for (Instruction &inst : make_early_inc_range(bb)) {
      auto *call = dyn_cast<CallInst>(&inst);
      Function *start = call ? call->getCalledFunction() : nullptr;
      if (!start || !StringRef(llvm::demangle(start->getName().str())).starts_with("std::rt::lang_start::<") ||
          call->arg_empty())
        continue;
      auto *entry = dyn_cast<Function>(call->getArgOperand(0)->stripPointerCasts());
      if (!entry || !entry->arg_empty())
        continue;
      IRBuilder<> builder(call);
      builder.CreateCall(entry);
      call->replaceAllUsesWith(Constant::getNullValue(call->getType()));
      call->eraseFromParent();
    }
}

// rustc mangles the allocator shims (__rust_alloc and the rest) as
// __rustc::__rust_alloc with a hash that depends on the compiler build;
// give them their plain names so models/rust.c defines them.
static void renameRustShims(Module &m) {
  for (Function &f : m) {
    if (!f.isDeclaration() || !f.getName().starts_with("_R"))
      continue;
    char *raw = llvm::rustDemangle(f.getName());
    StringRef name = raw ? StringRef(raw) : "";
    if (name.consume_front("__rustc::") && name.starts_with("__rust_"))
      f.setName(name);
    std::free(raw);
  }
}

bool runsAtLoad(const GlobalValue &g) {
  StringRef s = isa<GlobalObject>(g) ? cast<GlobalObject>(g).getSection() : "";
  return isa<GlobalIFunc>(g) || g.getName() == "llvm.global_ctors" ||
         g.getName() == "llvm.global_dtors" || s.contains("init_array") ||
         s.contains("fini_array") || s.contains("ctors") || s.contains("dtors");
}

// Drops functions, globals, aliases and ifuncs not reachable from entry
// (plus the module's used lists and what the loader runs) before the passes
// and model linking run.
static void pruneUnreachable(Module &m, StringRef entry) {
  if (entry.empty())
    return;
  Function *root = m.getFunction(entry);
  if (!root || root->isDeclaration())
    return;

  SmallPtrSet<const GlobalValue *, 32> reachable;
  SmallPtrSet<const Constant *, 32> seenConsts;
  SmallVector<const GlobalValue *, 64> work;
  auto mark = [&](auto &self, const Value *v) -> void {
    if (auto *gv = dyn_cast<GlobalValue>(v)) {
      if (reachable.insert(gv).second)
        work.push_back(gv);
    } else if (auto *c = dyn_cast<Constant>(v)) {
      if (!isa<ConstantData>(c) && seenConsts.insert(c).second)
        for (const Use &u : c->operands())
          self(self, u.get());
    }
  };

  mark(mark, root);
  for (StringRef special : {"llvm.used", "llvm.compiler.used"})
    if (GlobalVariable *g = m.getNamedGlobal(special))
      mark(mark, g);
  for (GlobalValue &g : m.global_values())
    if (runsAtLoad(g))
      mark(mark, &g);
  // A C library model linked later may call a library function the program
  // defines, as realloc calls malloc; the program's definition must stay, or
  // the model's or the walker's would replace it.
  TargetLibraryInfoImpl tlii(m.getTargetTriple());
  LibFunc lf;
  for (Function &f : m)
    if (!f.isDeclaration() && !f.hasLocalLinkage() && tlii.getLibFunc(f.getName(), lf))
      mark(mark, &f);

  while (!work.empty()) {
    const GlobalValue *gv = work.pop_back_val();
    if (auto *f = dyn_cast<Function>(gv)) {
      if (f->hasPersonalityFn())
        mark(mark, f->getPersonalityFn());
      if (f->hasPrefixData())
        mark(mark, f->getPrefixData());
      if (f->hasPrologueData())
        mark(mark, f->getPrologueData());
      for (const Instruction &inst : instructions(*f))
        for (const Use &u : inst.operands())
          mark(mark, u.get());
    } else if (auto *g = dyn_cast<GlobalVariable>(gv)) {
      if (g->hasInitializer())
        mark(mark, g->getInitializer());
    } else if (auto *a = dyn_cast<GlobalAlias>(gv)) {
      if (const Constant *aliasee = a->getAliasee())
        mark(mark, aliasee);
    } else if (auto *ifunc = dyn_cast<GlobalIFunc>(gv)) {
      if (const Constant *resolver = ifunc->getResolver())
        mark(mark, resolver);
    }
  }

  SmallVector<Function *, 32> deadFns;
  SmallVector<GlobalVariable *, 32> deadGlobals;
  SmallVector<GlobalAlias *, 8> deadAliases;
  SmallVector<GlobalIFunc *, 4> deadIFuncs;
  for (Function &f : m)
    if (!reachable.contains(&f))
      deadFns.push_back(&f);
  for (GlobalVariable &g : m.globals())
    if (!reachable.contains(&g))
      deadGlobals.push_back(&g);
  for (GlobalAlias &a : m.aliases())
    if (!reachable.contains(&a))
      deadAliases.push_back(&a);
  for (GlobalIFunc &ifunc : m.ifuncs())
    if (!reachable.contains(&ifunc))
      deadIFuncs.push_back(&ifunc);

  for (Function *f : deadFns)
    f->dropAllReferences();
  for (GlobalVariable *g : deadGlobals)
    if (g->hasInitializer())
      g->setInitializer(nullptr);
  for (GlobalAlias *a : deadAliases)
    a->setAliasee(nullptr);
  for (GlobalIFunc *ifunc : deadIFuncs)
    ifunc->setResolver(nullptr);

  for (GlobalAlias *a : deadAliases) {
    a->removeDeadConstantUsers();
    if (a->use_empty())
      a->eraseFromParent();
  }
  for (GlobalIFunc *ifunc : deadIFuncs) {
    ifunc->removeDeadConstantUsers();
    if (ifunc->use_empty())
      ifunc->eraseFromParent();
  }
  for (Function *f : deadFns) {
    f->removeDeadConstantUsers();
    if (f->use_empty())
      f->eraseFromParent();
  }
  for (GlobalVariable *g : deadGlobals) {
    g->removeDeadConstantUsers();
    if (g->use_empty())
      g->eraseFromParent();
  }
}

// Threads trivial i1-phi merge blocks (clang's land.end / lor.end for && and
// ||) whose predecessor supplies a constant boolean, so loops like
// `while (y < 1000 && nondet())` expose `y < 1000` as a direct loop exit to
// SCEV without running full simplifycfg or disturbing witness block names.
static void threadBoolPhis(Module &m) {
  for (Function &f : m) {
    if (f.isDeclaration())
      continue;
    bool changed = true;
    while (changed) {
      changed = false;
      for (BasicBlock &bb : make_early_inc_range(f)) {
        auto *br = dyn_cast<CondBrInst>(bb.getTerminator());
        if (!br)
          continue;
        auto *phi = dyn_cast<PHINode>(br->getCondition());
        if (!phi || phi->getParent() != &bb || !phi->hasOneUse())
          continue;
        if (&bb.front() != phi || phi->getNextNode() != br)
          continue;
        SmallVector<BasicBlock *, 4> toThread;
        for (unsigned i = 0; i < phi->getNumIncomingValues(); ++i)
          if (isa<ConstantInt>(phi->getIncomingValue(i)))
            toThread.push_back(phi->getIncomingBlock(i));
        for (BasicBlock *pred : toThread) {
          int idx = phi->getBasicBlockIndex(pred);
          if (idx < 0)
            continue;
          auto *ci = cast<ConstantInt>(phi->getIncomingValue(idx));
          BasicBlock *target = ci->isOne() ? br->getSuccessor(0) : br->getSuccessor(1);
          if (target == &bb || pred == &bb || is_contained(successors(pred), target))
            continue;
          Instruction *term = pred->getTerminator();
          unsigned count = 0;
          for (unsigned s = 0; s < term->getNumSuccessors(); ++s)
            if (term->getSuccessor(s) == &bb)
              ++count;
          if (count != 1)
            continue;
          for (unsigned s = 0; s < term->getNumSuccessors(); ++s)
            if (term->getSuccessor(s) == &bb)
              term->setSuccessor(s, target);
          for (PHINode &tp : target->phis())
            tp.addIncoming(tp.getIncomingValueForBlock(&bb), pred);
          phi->removeIncomingValue(idx, /*DeletePHIIfEmpty=*/false);
          changed = true;
        }
        if (phi->getNumIncomingValues() == 0) {
          for (BasicBlock *succ : successors(&bb))
            succ->removePredecessor(&bb);
          bb.eraseFromParent();
          changed = true;
        } else if (phi->getNumIncomingValues() == 1) {
          br->setCondition(phi->getIncomingValue(0));
          phi->eraseFromParent();
          changed = true;
        }
      }
    }
  }
}

Expected<std::unique_ptr<Module>> load(StringRef path, LLVMContext &ctx, StringRef models,
                                       StringRef entry, bool divByZeroCheck, bool countZeroCheck,
                                       bool floatCastOverflowCheck, bool alignmentCheck,
                                       bool uninitCheck) {
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

  pruneUnreachable(*module, entry);
  stripUBAssumptions(*module);
  recordSanitizers(*module);
  nameSanitizerChecks(*module);
  lowerRustMain(*module);
  renameRustShims(*module);
  if (alignmentCheck)
    checkAlignments(*module, compiledByClang(*module));
  nondetLocals(*module, uninitCheck);
  lowerLongjmp(*module);
  if (divByZeroCheck)
    checkDivisors(*module);
  if (countZeroCheck)
    checkCountZeros(*module);
  if (floatCastOverflowCheck)
    checkFloatCasts(*module);
  if (Error e = runPipeline(*module, "function(mem2reg,sroa)"))
    return std::move(e);
  lowerCalls(*module); // before InstCombine, which folds tests of constants
  libmToIntrinsics(*module);
  lowerInlineAsm(*module);
  // Plain instcombine; the plan wants a "careful" one that folds
  // nothing on undefined behaviour. Revisit when a folded check shows up.
  // no-verify-fixpoint, as LLVM's own pipelines run it: a textual pipeline
  // otherwise aborts when InstCombine needs more than one iteration.
  SmallVector<GlobalVariable *, 4> written = unconstantWritten(*module);
  if (Error e = runPipeline(*module,
                            "function(instcombine<no-verify-fixpoint>,"
                            "scalarizer<load-store;variable-insert-extract>)"))
    return std::move(e);
  dropSafeChecks(*module, "__vereda_check_divisor");
  dropSafeChecks(*module, "__vereda_check_count_zero");
  dropSafeChecks(*module, "__vereda_check_float_cast_overflow");
  for (GlobalVariable *g : written)
    g->setConstant(true);
  // InstCombine drops safe sanitizer calls; discard their diagnostic globals
  // too, before their pointer initialisers add irrelevant layout constraints.
  pruneUnreachable(*module, entry);
  // After InstCombine, which sets fixIrreducible's guard on a way in from a
  // block it finds dead, or reached only by undefined behaviour, to poison,
  // where the walker may still walk; before loop-simplify and lcssa, which
  // shape its loops too. inferattrs, last, gives C library declarations the
  // attributes that tell the walker which arguments a bodyless call only
  // reads.
  threadBoolPhis(*module);
  fixIrreducible(*module);
  if (Error e = runPipeline(*module, "function(loop-simplify,lcssa),inferattrs"))
    return std::move(e);
  stripUBAssumptions(*module);
  dropUninitialisedRecords(*module);
  if (Error e = linkModels(*module, models, divByZeroCheck, countZeroCheck, floatCastOverflowCheck,
                           alignmentCheck))
    return std::move(e);
  // Vereda intercepts libc assertion failures and never passes their
  // string arguments outside; mark its pointer parameters non-capturing before
  // function-attrs so callers do not appear to expose assert message literals.
  for (Function &af : *module) {
    if (!af.isDeclaration() || !isAssertFail(af.getName(), af.arg_size()))
      continue;
    Attribute noCap = Attribute::getWithCaptureInfo(module->getContext(), CaptureInfo::none());
    for (Argument &arg : af.args())
      if (arg.getType()->isPointerTy())
        arg.addAttr(noCap);
  }
  // Infer captures(none) on defined functions and linked models (e.g. strcmp)
  // so globals passed only to non-capturing calls are not marked exposed.
  if (Error e = runPipeline(*module, "cgscc(function-attrs)"))
    return std::move(e);
  stripUBAssumptions(*module);
  return module;
}

} // namespace vereda::ir
