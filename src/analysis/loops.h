// Stage analysis: the loops the walker replaces by their effect instead of
// unrolling them (issue #36), after ESBMC's array loop summaries
// (esbmc/esbmc#10563). Such a loop exits only at its header, after a number
// of iterations SCEV computes, or at a block that reports a violation; its
// header phis advance by constant steps; and its body has no side effect but
// checks, nondet values and at most one store, to a[i] of an array of its
// own. Each iteration then depends only on its index, so the walker runs one
// iteration at a nondet index, which keeps every check, and writes the
// store's effect on the whole array at once. A loop that may not have this
// shape is unrolled as before.
#pragma once
#include <llvm/ADT/DenseMap.h>
#include <llvm/Analysis/AssumptionCache.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/ScalarEvolutionExpressions.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/Dominators.h>
#include <llvm/IR/Instructions.h>
#include <optional>

namespace vereda::analysis {

// Which ends of a path report a violation under the property checked; any
// other end of a path in a loop body stops the loop early, which a summary
// cannot express.
struct Counted {
  bool assertions; // assert, __VERIFIER_assert, __assert_fail, a bodyless reach_error
  bool traps;      // llvm.ubsantrap, a clang sanitizer check
  bool reachError; // reach_error by name, under unreach-call
};

struct Summary {
  const llvm::SCEV *trips; // iterations of the body: the header's exit count
  llvm::BasicBlock *exit;  // where the header leaves the loop
  // The one store, if any, at {first,+,stride} with stride at least its
  // size, into an object no other access in the loop reads but at the same
  // address.
  llvm::StoreInst *store = nullptr;
  const llvm::SCEVAddRecExpr *address = nullptr;
  uint64_t stride = 0;
  bool nondet = false; // the body calls a __VERIFIER_nondet_ function
};

class Loops {
public:
  Loops(llvm::Function &f, llvm::DominatorTree &dt, llvm::LoopInfo &li, Counted counted);
  llvm::ScalarEvolution &scev() { return se; }
  // l's summary, or null when l may not have the shape above.
  const Summary *summary(const llvm::Loop *l);

private:
  std::optional<Summary> match(const llvm::Loop *l);
  bool callOk(const llvm::CallBase &c, unsigned depth, bool reads) const;
  bool sideEffectFree(const llvm::Instruction &inst, unsigned depth, bool reads) const;
  const llvm::Value *base(const llvm::Value *pointer, const llvm::Loop *l);

  llvm::TargetLibraryInfoImpl tlii;
  llvm::TargetLibraryInfo tli;
  llvm::AssumptionCache ac;
  llvm::ScalarEvolution se;
  Counted counted;
  bool threads; // the program creates threads: no summaries
  llvm::DenseMap<const llvm::Loop *, std::optional<Summary>> cache;
};

} // namespace vereda::analysis
