#include "analysis/loops.h"

#include <llvm/Analysis/CFG.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/Module.h>

using namespace llvm;

namespace vereda::analysis {

Loops::Loops(Function &f, DominatorTree &dt, LoopInfo &li, Counted counted)
    : tlii(f.getParent()->getTargetTriple()), tli(tlii, &f), ac(f), se(f, tli, ac, dt, li),
      counted(counted) {
  // A summary writes the whole array at once, which another thread could
  // see half written.
  const Function *create = f.getParent()->getFunction("pthread_create");
  threads = create && !create->use_empty();
}

const Summary *Loops::summary(const Loop *l) {
  auto it = cache.find(l);
  if (it == cache.end())
    it = cache.try_emplace(l, match(l)).first;
  return it->second ? &*it->second : nullptr;
}

// The walker encodes floats with the solver's own terms, which a summary
// cannot rewrite for another index.
static bool floating(const Instruction &inst) {
  auto fp = [](Type *ty) { return ty->getScalarType()->isFloatingPointTy(); };
  return fp(inst.getType()) || any_of(inst.operands(), [&](const Use &u) { return fp(u->getType()); });
}

// The function inst calls directly, if it is a call: a K&R call has a
// mismatched type, which only the cast hides.
static const Function *callee(const Instruction *inst) {
  auto *c = dyn_cast_or_null<CallBase>(inst);
  return c ? dyn_cast<Function>(c->getCalledOperand()->stripPointerCasts()) : nullptr;
}

static bool isErrorCall(const Instruction *inst) {
  const Function *f = callee(inst);
  StringRef name = f ? f->getName() : "";
  return name == "reach_error" || name == "__assert_fail" || name == "__VERIFIER_error";
}

// Whether a call in a loop body is a check, a nondet value (depth 0, the
// body itself), or a call to a function that is itself free of effects:
// reads says whether it may read memory.
bool Loops::callOk(const CallBase &c, unsigned depth, bool reads) const {
  const Function *f = callee(&c);
  if (!f)
    return false;
  StringRef name = f->getName();
  if (f->isIntrinsic())
    return name.starts_with("llvm.ubsantrap") ? counted.traps : f->doesNotAccessMemory();
  if (name == "reach_error" && counted.reachError)
    return true;
  // A loop in the callee may not end, which would stop the loop being
  // summarised part way.
  if (!f->isDeclaration()) {
    SmallVector<std::pair<const BasicBlock *, const BasicBlock *>, 1> backEdges;
    FindFunctionBackedges(*f, backEdges);
    if (depth >= 4 || f->isVarArg() || !backEdges.empty())
      return false;
    for (const Instruction &inst : instructions(*f))
      if (!sideEffectFree(inst, depth + 1, reads))
        return false;
    return true;
  }
  if (name.starts_with("__VERIFIER_nondet_"))
    return depth == 0 && c.getType()->isIntegerTy();
  if (name == "__VERIFIER_assert" || (name == "assert" && c.arg_size() == 1) || isErrorCall(&c))
    return counted.assertions;
  // The abort after a violation, which already ended the path.
  if (name == "abort" || name == "exit" || name == "_Exit") {
    if (!isErrorCall(c.getPrevNode()))
      return false;
    const Function *pf = callee(c.getPrevNode());
    if (pf->getName() == "reach_error" && counted.reachError)
      return true;
    return pf->isDeclaration() && counted.assertions;
  }
  return false;
}

bool Loops::sideEffectFree(const Instruction &inst, unsigned depth, bool reads) const {
  // Undef is a fresh value per use, which one iteration would fix for all.
  if (floating(inst) || any_of(inst.operands(), [](const Use &u) { return isa<UndefValue>(u); }))
    return false;
  if (auto *c = dyn_cast<CallBase>(&inst))
    return callOk(*c, depth, reads);
  if (auto *l = dyn_cast<LoadInst>(&inst))
    return reads && l->isSimple();
  if (isa<UnreachableInst>(inst)) { // as the walker reads it
    auto *prev = dyn_cast_or_null<CallBase>(inst.getPrevNode());
    return counted.assertions || (prev && prev->doesNotReturn());
  }
  if (inst.isTerminator())
    return isa<BranchInst>(inst) || isa<SwitchInst>(inst) || isa<ReturnInst>(inst);
  return !isa<AllocaInst>(inst) && !inst.mayHaveSideEffects() && !inst.mayReadFromMemory();
}

// The global, or alloca from before l, that p points into, if it is one.
const Value *Loops::base(const Value *p, const Loop *l) {
  auto *u = dyn_cast<SCEVUnknown>(se.getPointerBase(se.getSCEV(const_cast<Value *>(p))));
  const Value *v = u ? u->getValue() : nullptr;
  if (isa_and_nonnull<GlobalVariable>(v))
    return v;
  auto *a = dyn_cast_or_null<AllocaInst>(v);
  return a && !l->contains(a) ? a : nullptr;
}

std::optional<Summary> Loops::match(const Loop *l) {
  BasicBlock *header = l->getHeader();
  auto *br = dyn_cast<BranchInst>(header->getTerminator());
  if (threads || !l->isInnermost() || !l->getLoopPreheader() || !l->getLoopLatch() || !br ||
      !br->isConditional() || l->contains(br->getSuccessor(0)) == l->contains(br->getSuccessor(1)))
    return std::nullopt;
  Summary sum;
  sum.exit = br->getSuccessor(l->contains(br->getSuccessor(0)) ? 1 : 0);
  sum.trips = se.getExitCount(l, header);
  if (isa<SCEVCouldNotCompute>(sum.trips))
    return std::nullopt;

  // The header runs once more than the body, at the exit: no effect there.
  if (any_of(*header, [](Instruction &inst) { return isa<StoreInst>(inst) || isa<CallBase>(inst); }))
    return std::nullopt;
  const DataLayout &dl = header->getDataLayout();
  for (BasicBlock *bb : l->blocks())
    for (Instruction &inst : *bb)
      if (auto *st = dyn_cast<StoreInst>(&inst)) {
        if (sum.store)
          return std::nullopt;
        sum.store = st;
      }
  const Value *object = nullptr;
  uint64_t size = 0;
  if (sum.store) {
    Type *ty = sum.store->getValueOperand()->getType();
    sum.address = dyn_cast<SCEVAddRecExpr>(se.getSCEV(sum.store->getPointerOperand()));
    if (!sum.store->isSimple() || !ty->isIntegerTy() ||
        isa<UndefValue>(sum.store->getValueOperand()) || !sum.address ||
        sum.address->getLoop() != l || !sum.address->isAffine())
      return std::nullopt;
    auto *step = dyn_cast<SCEVConstant>(sum.address->getStepRecurrence(se));
    size = dl.getTypeStoreSize(ty);
    if (!step || !step->getAPInt().isStrictlyPositive() || step->getAPInt().getActiveBits() > 32 ||
        step->getAPInt().ult(size))
      return std::nullopt;
    sum.stride = step->getAPInt().getZExtValue();
    // Element n is at first + n * stride, with no two elements overlapping
    // even as offsets wrap.
    bool overflow;
    APInt bytes = se.getUnsignedRangeMax(sum.trips).zext(128).umul_ov(APInt(128, sum.stride), overflow);
    object = base(sum.store->getPointerOperand(), l);
    if (overflow || bytes.getActiveBits() > dl.getPointerSizeInBits() || !object)
      return std::nullopt;
  }

  // Each iteration reads memory as it was before the loop, but for its own
  // element: no access reads what another iteration writes.
  for (BasicBlock *bb : l->blocks())
    for (Instruction &inst : *bb) {
      if (&inst == sum.store)
        continue;
      auto *ld = dyn_cast<LoadInst>(&inst);
      if (!ld || !object) {
        if (!sideEffectFree(inst, 0, !object))
          return std::nullopt;
        const Function *f = callee(&inst);
        sum.nondet |= f && f->isDeclaration() && f->getName().starts_with("__VERIFIER_nondet_");
        continue;
      }
      const Value *from = base(ld->getPointerOperand(), l);
      if (floating(inst) || !ld->isSimple() || !from ||
          (from == object && (se.getSCEV(ld->getPointerOperand()) != sum.address ||
                              dl.getTypeStoreSize(ld->getType()) > size)))
        return std::nullopt;
    }

  // Any other way out reports a violation and ends the path there.
  SmallVector<BasicBlock *, 4> exits;
  l->getExitBlocks(exits);
  for (BasicBlock *bb : exits) {
    if (bb == sum.exit)
      continue;
    if (!isa<UnreachableInst>(bb->getTerminator()))
      return std::nullopt;
    for (Instruction &inst : *bb)
      if (!sideEffectFree(inst, 0, !object))
        return std::nullopt;
  }
  if (any_of(predecessors(sum.exit), [&](BasicBlock *p) { return p != header; }))
    return std::nullopt;
  return sum;
}

} // namespace vereda::analysis
