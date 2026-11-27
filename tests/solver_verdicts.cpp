// Non-incremental solving keeps a violation it found and never reports one
// the solver's model does not show, whatever a later query answers.
// Incremental solving asks the solver nothing about a constant unwinding check
// under --incremental-bmc.
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

// Bitwuzla that counts its checkSatAssuming calls.
struct Counting : camada::BitwuzlaSolver {
  static inline unsigned calls = 0;
  camada::CheckResult checkSatAssumingImpl(const std::vector<camada::SMTExprRef> &a) override {
    ++calls;
    return camada::BitwuzlaSolver::checkSatAssumingImpl(a);
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

  // Under --incremental-bmc, an unwinding check that folds to true fails
  // without a solver call, which would put Bitwuzla in its incremental mode
  // before the first real query (#380); without the flag the solver is asked.
  {
    unsigned reported = 0;
    Counting::calls = 0;
    camada::SMTSolverRef s{new Counting()};
    encode::Terms t{*s};
    checks::Checks checks;
    checks.push_back({checks::Property::Unwinding, {}, "", "", 0, t.boolean(true)});
    Verdict v = solver::decide(t, checks, false, true, [&](const checks::Check &) { ++reported; });
    assert(Counting::calls == 0);
    assert(v.kind == Verdict::Violated);
    assert(v.check == &checks[0]);
    assert(v.status[0] == Status::Fails);
    assert(reported == 1);

    Counting::calls = 0;
    reported = 0;
    v = solver::decide(t, checks, false, false, [&](const checks::Check &) { ++reported; });
    assert(Counting::calls == 1);
    assert(v.kind == Verdict::Violated);
    assert(v.check == &checks[0]);
    assert(v.status[0] == Status::Fails);
    assert(reported == 1);

    Counting::calls = 0;
    reported = 0;
    checks.push_back({checks::Property::Unwinding, {}, "", "", 1, t.boolean(true)});
    v = solver::decide(t, checks, true, true, [&](const checks::Check &) { ++reported; });
    assert(Counting::calls == 0);
    assert(v.kind == Verdict::Violated);
    assert(v.check == &checks[0]);
    assert(v.status[0] == Status::Fails);
    assert(v.status[1] == Status::Fails);
    assert(reported == 2);
  }
}
