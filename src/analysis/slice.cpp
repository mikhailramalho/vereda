#include "analysis/slice.h"

#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/Transforms/Utils/Local.h>

using namespace llvm;

namespace vereda::analysis {

// Whether a pointer into obj may reach memory, a call, an integer, an
// aggregate or a return: anywhere the slice does not follow it, and where
// a read the slice cannot see may come from.
static bool escapes(const Value *obj) {
  SmallVector<const Value *, 8> work{obj};
  SmallPtrSet<const Value *, 8> seen{obj};
  while (!work.empty()) {
    const Value *v = work.pop_back_val();
    for (const Use &u : v->uses()) {
      const User *user = u.getUser();
      if (isa<LoadInst>(user))
        continue;
      // A comparison with another address exposes it, as the walker's
      // exposed() has it: fromAddress may then land there.
      if (auto *cmp = dyn_cast<ICmpInst>(user)) {
        if (isa<ConstantPointerNull>(cmp->getOperand(1 - u.getOperandNo())))
          continue;
        return true;
      }
      if (auto *s = dyn_cast<StoreInst>(user); s && u.getOperandNo() == s->getPointerOperandIndex())
        continue;
      if (auto *ii = dyn_cast<IntrinsicInst>(user); ii && ii->isLifetimeStartOrEnd())
        continue;
      auto *ce = dyn_cast<ConstantExpr>(user);
      bool derived =
          isa<GetElementPtrInst, BitCastInst, AddrSpaceCastInst, PHINode, SelectInst>(user) ||
          (ce && (isa<GEPOperator>(ce) || ce->getOpcode() == Instruction::BitCast ||
                  ce->getOpcode() == Instruction::AddrSpaceCast));
      if (!derived)
        return true;
      if (seen.insert(user).second)
        work.push_back(user);
    }
  }
  return false;
}

unsigned slice(Module &m, bool keepLoads, bool keepNondets) {
  // Objects the slice follows: an alloca, a writable global or an
  // allocation whose address does not escape, so that every read of it is
  // a load whose underlying objects include it. Each is mapped to the
  // stores into it, which matter once some kept load reads it.
  DenseMap<const Value *, bool> tracked;
  auto isTracked = [&](const Value *o) {
    auto [it, inserted] = tracked.try_emplace(o, false);
    if (inserted) {
      auto *g = dyn_cast<GlobalVariable>(o);
      it->second = (isa<AllocaInst>(o) || (g && !g->isConstant()) || isNoAliasCall(o)) &&
                   !escapes(o);
    }
    return it->second;
  };
  auto objects = [](const Value *p) {
    SmallVector<const Value *, 4> objs;
    getUnderlyingObjects(p, objs, nullptr, /*MaxLookup=*/0);
    return objs;
  };

  DenseSet<Instruction *> live;
  SmallVector<Instruction *, 64> work;
  DenseSet<const Value *> read;
  DenseMap<const Value *, SmallVector<StoreInst *, 4>> storesInto;
  auto keep = [&](Value *v) {
    if (auto *i = dyn_cast<Instruction>(v); i && live.insert(i).second)
      work.push_back(i);
  };

  // Loops, and the functions they call, stay whole: what a loop body holds
  // decides whether the walker summarises the loop (analysis/loops.h),
  // which then has no unwinding assertion, and whether it ends the paths
  // into an endless loop. Slicing must change neither.
  SmallVector<Function *, 16> called;
  SmallPtrSet<Function *, 16> whole;
  auto keepWhole = [&](Instruction &i) {
    keep(&i);
    if (auto *c = dyn_cast<CallBase>(&i))
      if (auto *f = dyn_cast<Function>(c->getCalledOperand()->stripPointerCasts());
          f && whole.insert(f).second)
        called.push_back(f);
  };
  for (Function &f : m) {
    if (f.isDeclaration())
      continue;
    DominatorTree dt(f);
    LoopInfo li(dt);
    for (Loop *l : li)
      for (BasicBlock *bb : l->blocks())
        for (Instruction &i : *bb)
          keepWhole(i);
  }
  while (!called.empty())
    for (Instruction &i : instructions(*called.pop_back_val()))
      keepWhole(i);

  for (Function &f : m)
    for (Instruction &i : instructions(f)) {
      if (auto *s = dyn_cast<StoreInst>(&i)) {
        SmallVector<const Value *, 4> objs = objects(s->getPointerOperand());
        // Under the uninit check, a load whose value is only stored defers
        // its check to the copy's reads (the walker's copyOnly()).
        if (keepLoads || s->isVolatile() || !all_of(objs, isTracked)) {
          keep(s);
          continue;
        }
        for (const Value *o : objs)
          storesInto[o].push_back(s);
        continue;
      }
      if (auto *l = dyn_cast<LoadInst>(&i)) {
        if (keepLoads || l->isVolatile())
          keep(l);
        continue;
      }
      if (auto *a = dyn_cast<AllocaInst>(&i)) {
        if (!isa<ConstantInt>(a->getArraySize())) // checked against PTRDIFF_MAX
          keep(a);
        continue;
      }
      if (auto *c = dyn_cast<CallBase>(&i)) {
        // A bodyless __VERIFIER_nondet_ call only makes a value.
        Function *callee = c->getCalledFunction();
        bool nondet = callee && callee->isDeclaration() &&
                      callee->getName().starts_with("__VERIFIER_nondet_") &&
                      callee->getName() != "__VERIFIER_nondet_memory";
        if (keepNondets || !nondet)
          keep(c);
        continue;
      }
      if (i.isTerminator() || i.mayHaveSideEffects() || i.isEHPad())
        keep(&i);
    }

  while (!work.empty()) {
    Instruction *i = work.pop_back_val();
    for (Value *op : i->operands())
      keep(op);
    if (auto *l = dyn_cast<LoadInst>(i))
      for (const Value *o : objects(l->getPointerOperand()))
        if (read.insert(o).second)
          for (StoreInst *s : storesInto.lookup(o))
            keep(s);
  }

  SmallVector<Instruction *, 64> dead;
  for (Function &f : m)
    for (Instruction &i : instructions(f))
      if (!live.contains(&i))
        dead.push_back(&i);
  for (Instruction *i : dead) {
    salvageDebugInfo(*i);
    if (!i->use_empty())
      i->replaceAllUsesWith(PoisonValue::get(i->getType()));
    i->dropAllReferences();
  }
  for (Instruction *i : dead)
    i->eraseFromParent();
  return dead.size();
}

} // namespace vereda::analysis
