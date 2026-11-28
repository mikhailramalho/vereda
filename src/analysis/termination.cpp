#include "analysis/termination.h"
#include "analysis/loops.h"
#include "ir/debug.h"
#include "ir/load.h"

#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/Analysis/CFG.h>
#include <llvm/Analysis/ValueTracking.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/InstIterator.h>
#include <llvm/IR/IntrinsicInst.h>
#include <llvm/IR/Module.h>

using namespace llvm;

namespace vereda::analysis {

namespace {
struct Prover {
  TargetLibraryInfoImpl tlii;
  // Some defined function's address is taken: a bodyless function given it
  // (qsort, atexit, pthread_create, signal) may call it.
  bool callbacks;
  DenseMap<const Function *, bool> visited{}; // false while on the call stack
  Termination result{};

  void visit(Function &f);
  static void boundInclusive(Loop &l, const DominatorTree &dt);
  void call(CallBase &c);
  static void note(std::string &why, const Twine &reason) {
    if (why.empty())
      why = reason.str();
  }
};
} // namespace

// Bodyless functions that call no function of the program: the verifier
// builtins, Vereda's checks and helpers, the C library's ends of a run and
// the allocator, which the walker models. Not __vereda_unknown_target, the
// target of an indirect call ir could not match, which may be a function of
// the program.
static bool callsNothing(StringRef name) {
  return name.starts_with("__VERIFIER_") || name.starts_with("__ubsan_handle_") ||
         is_contained<StringRef>({"__vereda_check_alignment", "__vereda_check_count_zero",
                                  "__vereda_check_divisor", "__vereda_check_float_cast_overflow",
                                  "__vereda_havoc", "__vereda_object_size",
                                  "__vereda_uninitialised", "__vereda_longjmp_unmatched",
                                  "__vereda_error_label", "__vereda_pclose_not_popen", "abort",
                                  "exit", "_Exit", "__assert_fail", "reach_error", "malloc",
                                  "calloc", "realloc", "free"},
                                 name);
}

// A jump the CFG does not show: a setjmp or longjmp ir did not lower, one it
// gave up on, or the builtin and coroutine intrinsics.
static bool jumps(StringRef name) {
  return name.starts_with("llvm.eh.") || name.starts_with("llvm.coro.") ||
         is_contained<StringRef>({"setjmp", "_setjmp", "__sigsetjmp", "sigsetjmp", "longjmp",
                                  "_longjmp", "siglongjmp", "__longjmp_chk",
                                  "__vereda_longjmp_unsupported"},
                                 name);
}

void Prover::call(CallBase &c) {
  auto *callee = dyn_cast<Function>(c.getCalledOperand()->stripPointerCasts());
  std::string at = " at " + ir::location(c).display();
  // ir lowers indirect calls to direct ones and the inline assembly it knows.
  if (!callee)
    return note(result.unseen, "indirect call or inline assembly" + at);
  // The walker refuses both, so it cannot vouch for them either.
  if (isa<InvokeInst>(c) || (!c.getCalledFunction() && !callee->isDeclaration()))
    return note(result.unseen, "invoke or call with a mismatched signature" + at);
  if (jumps(callee->getName()))
    return note(result.unseen, "non-local jump" + at);
  // SCEV would take the condition as known, which C leaves undefined when false.
  if (callee->getIntrinsicID() == Intrinsic::assume)
    return note(result.unbounded, "__builtin_assume" + at);
  if (!callee->isDeclaration())
    return visit(*callee);
  // A bodyless function that does not return blocks, which does not violate
  // termination (sv-benchmarks' properties/termination.md); one that calls
  // back into the program might run a loop of it.
  if (callbacks && !callee->isIntrinsic() && !callsNothing(callee->getName()))
    note(result.unseen, "call to " + demangle(callee->getName()) + at +
                            ", which may call a function whose address is taken");
}

// SCEV counts a loop that runs while i <= n only when n's range excludes the
// maximum, and it takes no range from a guard (#362). Where a guard implies
// n != max, i <= n is i < n + 1 without a wrap, which SCEV counts; the exit
// test is rewritten so (and i > n as i >= n + 1), in place, so a walk that
// follows sees the same loop. The guard is the branch into the preheader or
// into a block that dominates it, found by walking up the dominator tree
// (#481): isImpliedByDomCondition looks only at a block's single
// predecessor.
void Prover::boundInclusive(Loop &l, const DominatorTree &dt) {
  BasicBlock *pre = l.getLoopPreheader();
  if (!pre)
    return;
  SmallVector<BasicBlock *> exiting;
  l.getExitingBlocks(exiting);
  for (BasicBlock *b : exiting) {
    auto *br = dyn_cast<CondBrInst>(b->getTerminator());
    auto *cmp = br ? dyn_cast<ICmpInst>(br->getCondition()) : nullptr;
    if (!cmp || !cmp->getOperand(0)->getType()->isIntegerTy())
      continue;
    if (l.isLoopInvariant(cmp->getOperand(0)) && !l.isLoopInvariant(cmp->getOperand(1)))
      cmp->swapOperands();
    Value *n = cmp->getOperand(1);
    ICmpInst::Predicate pred = cmp->getPredicate();
    if (!l.isLoopInvariant(n) || !is_contained({ICmpInst::ICMP_SGT, ICmpInst::ICMP_SLE,
                                                ICmpInst::ICMP_UGT, ICmpInst::ICMP_ULE},
                                               pred))
      continue;
    unsigned w = n->getType()->getIntegerBitWidth();
    APInt max = ICmpInst::isSigned(pred) ? APInt::getSignedMaxValue(w) : APInt::getMaxValue(w);
    Constant *maxc = ConstantInt::get(n->getType(), max);
    const DomTreeNode *d = dt.getNode(pre);
    while (d && isImpliedByDomCondition(ICmpInst::ICMP_NE, n, maxc, d->getBlock()->getTerminator(),
                                        pre->getDataLayout()) != true)
      d = d->getIDom();
    if (!d)
      continue;
    cmp->setOperand(1, BinaryOperator::CreateAdd(n, ConstantInt::get(n->getType(), 1), "",
                                                 pre->getTerminator()->getIterator()));
    cmp->setPredicate(ICmpInst::getFlippedStrictnessPredicate(pred));
  }
}

void Prover::visit(Function &f) {
  if (auto it = visited.find(&f); it != visited.end()) {
    if (!it->second)
      note(result.unbounded, "recursion through " + demangle(f.getName()));
    return;
  }
  visited[&f] = false;
  DominatorTree dt(f);
  LoopInfo li(dt);
  // ir makes every cycle a natural loop; one that is not would escape LoopInfo.
  ReversePostOrderTraversal<Function *> rpot(&f);
  if (containsIrreducibleCFG<BasicBlock *>(rpot, li))
    note(result.unseen, "irreducible control flow in " + demangle(f.getName()));
  Loops loops(f, dt, li, tlii, {}, false);
  for (Loop *l : li.getLoopsInPreorder())
    boundInclusive(*l, dt);
  for (Loop *l : li.getLoopsInPreorder())
    if (isa<SCEVCouldNotCompute>(loops.scev().getSymbolicMaxBackedgeTakenCount(l)))
      note(result.unbounded, "SCEV cannot bound the loop at " +
                                 ir::location(l->getStartLoc(), f.getName()).display());
  for (Instruction &inst : instructions(f))
    if (auto *c = dyn_cast<CallBase>(&inst))
      call(*c);
  visited[&f] = true;
}

Termination termination(Function &entry) {
  Module &m = *entry.getParent();
  Prover p{TargetLibraryInfoImpl(m.getTargetTriple()),
           any_of(m, [](Function &f) { return !f.isDeclaration() && f.hasAddressTaken(); })};
  // ir keeps them all, used or not.
  if (any_of(m.global_values(), [](GlobalValue &g) { return ir::runsAtLoad(g); }))
    Prover::note(p.result.unseen, "constructors, destructors or ifuncs");
  // Module-level assembly can list functions in those sections too
  // (.pushsection .init_array), where no global shows them.
  if (!m.getModuleInlineAsm().empty())
    Prover::note(p.result.unseen, "module-level assembly");
  p.visit(entry);
  Prover::note(p.result.unbounded, p.result.unseen);
  return p.result;
}

} // namespace vereda::analysis
