// Non-incremental solving keeps a violation it found and never reports one
// the solver's model does not show, whatever a later query answers.
#undef NDEBUG
#include "solver/solver.h"
#include <camada/solvers/bitwuzlasolver.h>
#include <cassert>
#include <memory>

using namespace vereda;
using solver::Status;
using solver::Verdict;

namespace {
// Bitwuzla that answers unknown from the unknownFrom-th check on, and whose
// model, with noModel, satisfies no term asked about.
struct Faulty : camada::BitwuzlaSolver {
  static inline unsigned checksMade = 0, unknownFrom = ~0u;
  static inline bool noModel = false;
  camada::CheckResult checkImpl() override {
    return ++checksMade >= unknownFrom ? camada::CheckResult::UNKNOWN
                                       : camada::BitwuzlaSolver::checkImpl();
  }
  camada::SMTResult<bool> getBoolImpl(const camada::SMTExprRef &e) override {
    return noModel ? camada::SMTResult<bool>(false) : camada::BitwuzlaSolver::getBoolImpl(e);
  }
};

// A solver, its terms, and two checks: x == 1, then x == 2 of the given property.
struct Query {
  camada::SMTSolverRef s{new Faulty()};
  encode::Terms t{*s};
  checks::Checks checks;
  explicit Query(checks::Property second) {
    encode::Term x = t.symbol("x", s->mkBVSort(32));
    checks.push_back({checks::Property::Assertion, {}, "", "", 1, t.eq(x, t.bv(1, 32))});
    checks.push_back({second, {}, "", "", 2, t.eq(x, t.bv(2, 32))});
  }
};

Verdict decide(Query &q, bool all, std::unique_ptr<Query> &fresh) {
  bool used;
  return solver::decideNonIncremental(
      q.t, q.checks, all, /*skipUnwindModel=*/false,
      [&]() -> std::pair<encode::Terms *, const checks::Checks *> {
        fresh = std::make_unique<Query>(q.checks[1].property);
        return {&fresh->t, &fresh->checks};
      },
      [](encode::Terms &, const checks::Checks &, size_t) {}, used);
}
} // namespace

int main() {
  // The assertion fails; the unwinding phase, on a fresh solver, answers
  // unknown. The failure stands and the unwinding assertion is not checked.
  {
    Faulty::checksMade = 0;
    Faulty::unknownFrom = 2;
    Query q(checks::Property::Unwinding);
    std::unique_ptr<Query> fresh;
    Verdict v = decide(q, /*all=*/true, fresh);
    assert(fresh);
    assert(v.kind == Verdict::Violated && v.check == &q.checks[0]);
    assert(v.status[0] == Status::Fails && v.status[1] == Status::NotChecked);
  }
  // Satisfiable, but the model violates neither check: that is no failure.
  for (bool all : {false, true}) {
    Faulty::checksMade = 0;
    Faulty::unknownFrom = ~0u;
    Faulty::noModel = true;
    Query q(checks::Property::Assertion);
    std::unique_ptr<Query> fresh;
    Verdict v = decide(q, all, fresh);
    assert(v.kind == Verdict::Unknown);
    assert(v.status[0] == Status::NotChecked && v.status[1] == Status::NotChecked);
  }
}
