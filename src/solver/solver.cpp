#include "solver/solver.h"

#include <camada/camadafeatures.h>
#if CAMADA_HAVE_BITWUZLA
#include <camada/solvers/bitwuzlasolver.h>
#endif

using namespace llvm;

namespace vereda::solver {

#if CAMADA_HAVE_BITWUZLA
namespace {
// Bitwuzla with bv-cnf-scope=permanent. Each checkSatAssuming pushes a level
// for its assumption. Under the default, adaptive, the clauses that define the
// AIG nodes a check needs mostly take that level, so they carry its activation
// literal and go at the pop, to be encoded again by the next check that needs
// them. Permanent adds them at level 0 and keeps them; only the clause that
// asserts the assumption takes the level. Keeping them cannot change an
// answer: each defines a fresh variable from its inputs. Measured in
// docs/survey/2026-11-23-what-the-decrement-test-s-solve-depends-on.md.
struct Bitwuzla : camada::BitwuzlaSolver {
  Bitwuzla() { reset(); } // configureOptions applies only from here on
  void configureOptions(BitwuzlaOptions *o) override {
    bitwuzla_set_option_mode(o, BITWUZLA_OPT_BV_CNF_SCOPE, "permanent");
  }
};
} // namespace
#endif

Expected<camada::SMTSolverRef> create(camada::SolverKind kind) {
  switch (kind) {
#if CAMADA_HAVE_BITWUZLA
  case camada::SolverKind::Bitwuzla:
    return camada::SMTSolverRef(new Bitwuzla());
#endif
#if CAMADA_HAVE_Z3
  case camada::SolverKind::Z3:
    return camada::createZ3Solver();
#endif
#if CAMADA_HAVE_CVC5
  case camada::SolverKind::CVC5:
    return camada::createCVC5Solver();
#endif
  default:
    return createStringError(inconvertibleErrorCode(),
                             "this camada was built without that solver");
  }
}

Verdict decide(encode::Terms &t, const checks::Checks &checks, bool all,
               function_ref<void(const checks::Check &)> violated) {
  camada::SMTSolver &s = t.solver();
  // Assertions first, then unwinding assertions, as ESBMC's incremental BMC:
  // a bug within the bound is reported even where a loop is not unwound.
  Verdict v;
  v.status.assign(checks.size(), Status::NotChecked);
  for (bool unwinding : {false, true})
    for (size_t i = 0; i < checks.size(); ++i) {
      const checks::Check &check = checks[i];
      if (check.isUnwinding() != unwinding)
        continue;
      switch (s.checkSatAssuming({t.solverTerm(check.violated)})) {
      case camada::CheckResult::SAT:
        v.status[i] = Status::Fails;
        if (!v.check) {
          v.kind = Verdict::Violated;
          v.check = &check;
        }
        violated(check);
        if (!all)
          return v;
        break;
      case camada::CheckResult::UNSAT:
        v.status[i] = Status::Holds;
        break;
      case camada::CheckResult::UNKNOWN:
        if (v.check) // a violation found stands
          return v;
        return {Verdict::Unknown, &check, "solver answered unknown: " + s.reasonUnknownDetail(),
                std::move(v.status)};
      }
    }
  return v;
}

} // namespace vereda::solver
