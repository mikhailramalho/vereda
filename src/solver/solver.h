// Stage solver: camada glue. Owns the solver choice and decides the checks.
#pragma once
#include "checks/checks.h"
#include <camada/camada.h>
#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/Support/Error.h>
#include <string>

namespace vereda::solver {

// Bitwuzla, Z3 or CVC5, subject to what camada was built with. With
// incremental set, Bitwuzla uses bv-cnf-scope=permanent for assumption reuse.
llvm::Expected<camada::SMTSolverRef> create(camada::SolverKind kind, bool incremental = false);

enum class Status { NotChecked, Holds, Fails };

struct Verdict {
  enum Kind { Safe, Violated, Unknown } kind = Safe;
  const checks::Check *check = nullptr; // the first violated check, or the unknown one
  std::string detail;                   // reason, for Unknown
  std::vector<Status> status;           // per check, by index
};

// Incremental mode: asks one solver about each check under its own assumption,
// assertions before unwinding assertions. Calls violated on each satisfiable
// one while its model is available, and stops there unless all is set.
Verdict decide(encode::Terms &t, const checks::Checks &checks, bool all,
               llvm::function_ref<void(const checks::Check &)> violated);

// Non-incremental mode: asserts each phase's disjunction directly and calls
// check() without assumptions or push/pop. When a further query is needed
// (unwinding assertions after assertions held, or remaining checks under all),
// freshWalk supplies a fresh solver, terms and checks for the same walk (the
// same terms on a renewed solver, or those of a new walk).
// Sets solverUsed if at least one check() call was made on the initial solver.
Verdict decideNonIncremental(
    encode::Terms &t, const checks::Checks &checks, bool all, bool skipUnwindModel,
    llvm::function_ref<std::pair<encode::Terms *, const checks::Checks *>()> freshWalk,
    llvm::function_ref<void(encode::Terms &, const checks::Checks &, size_t)> violated,
    bool &solverUsed);

} // namespace vereda::solver
