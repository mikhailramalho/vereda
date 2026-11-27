#include "analysis/termination.h"
#include "analysis/loops.h"
#include "ir/debug.h"

#include <llvm/ADT/PostOrderIterator.h>
#include <llvm/Analysis/CFG.h>
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
  void call(CallBase &c);
  static void note(std::string &why, const Twine &reason) {
    if (why.empty())
      why = reason.str();
  }
};
} // namespace

// Bodyless functions that call no function of the program: Vereda's and the
// verifier builtins, the C library's ends of a run and the allocator, which
// the walker models.
static bool callsNothing(StringRef name) {
  return name.starts_with("__VERIFIER_") || name.starts_with("__vereda_") ||
         name.starts_with("__ubsan_handle_") ||
         is_contained<StringRef>({"abort", "exit", "_Exit", "__assert_fail", "reach_error",
                                  "malloc", "calloc", "realloc", "free"},
                                 name);
}

void Prover::call(CallBase &c) {
  auto *callee = dyn_cast<Function>(c.getCalledOperand()->stripPointerCasts());
  std::string at = " at " + ir::location(c).display();
  // ir lowers indirect calls to direct ones and the inline assembly it knows.
  if (!callee)
    return note(result.unseen, "indirect call or inline assembly" + at);
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
  // They run outside the entry's calls, as do the functions listed in a
  // section the loader runs.
  if (any_of(m.globals(), [](GlobalVariable &g) {
        StringRef s = g.getSection();
        return g.getName() == "llvm.global_ctors" || g.getName() == "llvm.global_dtors" ||
               s.contains("init_array") || s.contains("fini_array") || s.contains("ctors") ||
               s.contains("dtors");
      }))
    Prover::note(p.result.unseen, "constructors or destructors");
  p.visit(entry);
  Prover::note(p.result.unbounded, p.result.unseen);
  return p.result;
}

} // namespace vereda::analysis
