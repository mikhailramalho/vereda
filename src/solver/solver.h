// Stage solver: camada glue. Owns the solver choice and decides the checks.
#pragma once
#include "checks/checks.h"
#include <camada/camada.h>
#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/Support/Error.h>
#include <string>

namespace vereda::solver {

// Bitwuzla, Z3 or CVC5, subject to what camada was built with.
llvm::Expected<camada::SMTSolverRef> create(camada::SolverKind kind);

enum class Status { NotChecked, Holds, Fails };

struct Verdict {
  enum Kind { Safe, Violated, Unknown } kind = Safe;
  const checks::Check *check = nullptr; // the first violated check, or the unknown one
  std::string detail;                   // reason, for Unknown
  std::vector<Status> status;           // per check, by index
};

// Asks the solver about each check under its own assumption, assertions
// before unwinding assertions. Calls violated on each satisfiable one while
// its model is available, and stops there unless all is set.
Verdict decide(encode::Terms &t, const checks::Checks &checks, bool all,
               llvm::function_ref<void(const checks::Check &)> violated);

} // namespace vereda::solver
