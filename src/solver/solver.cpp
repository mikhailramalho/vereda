#include "solver/solver.h"

#include <algorithm>
#include <camada/camadafeatures.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallVector.h>
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

Expected<camada::SMTSolverRef> create(camada::SolverKind kind, bool incremental) {
  switch (kind) {
#if CAMADA_HAVE_BITWUZLA
  case camada::SolverKind::Bitwuzla:
    return incremental ? camada::SMTSolverRef(new Bitwuzla()) : camada::createBitwuzlaSolver();
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

// Balanced tree of ORs so a phase with many checks stays logarithmic in depth.
static camada::SMTExprRef disjunction(camada::SMTSolver &s, ArrayRef<camada::SMTExprRef> exprs) {
  if (exprs.size() == 1)
    return exprs[0];
  size_t mid = exprs.size() / 2;
  return s.mkOr(disjunction(s, exprs.take_front(mid)), disjunction(s, exprs.drop_front(mid)));
}

Verdict decide(encode::Terms &t, const checks::Checks &checks, bool all, bool skipUnwindModel,
               function_ref<void(const checks::Check &)> violated) {
  camada::SMTSolver &s = t.solver();
  // Assertions first, then unwinding assertions, as ESBMC's incremental BMC:
  // a bug within the bound is reported even where a loop is not unwound.
  Verdict v;
  v.status.assign(checks.size(), Status::NotChecked);

  struct Group {
    camada::SMTExprRef expr;
    SmallVector<size_t, 1> indices;
  };

  for (bool unwinding : {false, true}) {
    auto reportFirst = [&](size_t idx) {
      v.status[idx] = Status::Fails;
      v.kind = Verdict::Violated;
      v.check = &checks[idx];
      // Clear any constant-false checks after idx in this phase so !all
      // reports the exact same checked prefix as sequential solving.
      for (size_t j = idx + 1; j < checks.size(); ++j)
        if (checks[j].isUnwinding() == unwinding)
          v.status[j] = Status::NotChecked;
      violated(checks[idx]);
    };

    std::vector<Group> groups;
    DenseMap<const void *, size_t> byId;
    for (size_t i = 0; i < checks.size(); ++i) {
      const checks::Check &check = checks[i];
      if (check.isUnwinding() != unwinding)
        continue;
      if (t.isFalse(check.violated)) {
        v.status[i] = Status::Holds;
        continue;
      }
      // As in decideNonIncremental: a constant failure needs no solver call,
      // and a call would switch Bitwuzla to its incremental mode.
      if (unwinding && skipUnwindModel && t.isTrue(check.violated)) {
        if (!all) {
          reportFirst(i);
          return v;
        }
        v.status[i] = Status::Fails;
        if (!v.check) {
          v.kind = Verdict::Violated;
          v.check = &checks[i];
        }
        violated(checks[i]);
        continue;
      }
      auto [it, fresh] = byId.try_emplace(check.violated.id(), groups.size());
      if (fresh)
        groups.push_back({t.solverTerm(check.violated), {i}});
      else
        groups[it->second].indices.push_back(i);
    }

    auto checkIndividuallyPhase = [&](ArrayRef<Group> rem) -> bool {
      for (const Group &g : rem) {
        switch (encode::checked(s, s.checkSatAssuming({g.expr}))) {
        case camada::CheckResult::SAT:
          if (!all) {
            reportFirst(g.indices[0]);
            return true;
          }
          for (size_t idx : g.indices) {
            v.status[idx] = Status::Fails;
            if (!v.check) {
              v.kind = Verdict::Violated;
              v.check = &checks[idx];
            }
            violated(checks[idx]);
          }
          break;
        case camada::CheckResult::UNSAT:
          for (size_t idx : g.indices)
            v.status[idx] = Status::Holds;
          break;
        case camada::CheckResult::UNKNOWN:
          if (!v.check) {
            v.kind = Verdict::Unknown;
            v.check = &checks[g.indices[0]];
            v.detail = "solver answered unknown: " + s.reasonUnknownDetail();
          }
          return true;
        }
      }
      return false;
    };

    // In single-property mode (!all), solve deduplicated groups in order:
    // combining unrelated checks into one OR forces Bitwuzla to bit-blast
    // every check's cone of influence at once, which hurts both early failures
    // (e.g. 23_picosat-846_01: 2.5x slower, 1.9x memory) and multi-check UNSAT
    // proofs (e.g. lib_strtok) where bv-cnf-scope=permanent already shares
    // common clauses across sequential queries.
    if (!all) {
      if (checkIndividuallyPhase(groups))
        return v;
      continue;
    }

    while (!groups.empty()) {
      if (groups.size() == 1) {
        if (checkIndividuallyPhase(groups))
          return v;
        groups.clear();
        break;
      }
      std::vector<camada::SMTExprRef> exprs;
      exprs.reserve(groups.size());
      for (const Group &g : groups)
        exprs.push_back(g.expr);
      switch (encode::checked(s, s.checkSatAssuming({disjunction(s, exprs)}))) {
      case camada::CheckResult::UNSAT:
        for (const Group &g : groups)
          for (size_t idx : g.indices)
            v.status[idx] = Status::Holds;
        groups.clear();
        break;
      case camada::CheckResult::SAT: {
        std::vector<size_t> failed;
        std::vector<Group> next;
        for (Group &g : groups) {
          if (auto b = s.getBool(g.expr); b && b.value()) {
            failed.insert(failed.end(), g.indices.begin(), g.indices.end());
          } else {
            next.push_back(std::move(g));
          }
        }
        if (failed.empty()) {
          if (checkIndividuallyPhase(next))
            return v;
          groups.clear();
          break;
        }
        std::sort(failed.begin(), failed.end());
        for (size_t idx : failed) {
          v.status[idx] = Status::Fails;
          if (!v.check) {
            v.kind = Verdict::Violated;
            v.check = &checks[idx];
          }
          violated(checks[idx]);
        }
        groups = std::move(next);
        break;
      }
      case camada::CheckResult::UNKNOWN:
        if (checkIndividuallyPhase(groups))
          return v;
        groups.clear();
        break;
      }
    }
  }
  return v;
}

Verdict decideNonIncremental(
    encode::Terms &t, const checks::Checks &checks, bool all, bool skipUnwindModel,
    function_ref<std::pair<encode::Terms *, const checks::Checks *>()> freshWalk,
    function_ref<void(encode::Terms &, const checks::Checks &, size_t)> violated,
    bool &solverUsed) {
  Verdict v;
  v.status.assign(checks.size(), Status::NotChecked);
  solverUsed = false;

  encode::Terms *curT = &t;
  const checks::Checks *curChecks = &checks;
  bool currentDirty = false;

  auto ensureFresh = [&]() -> bool {
    if (!currentDirty)
      return true;
    auto [nt, nc] = freshWalk();
    if (!nt || !nc)
      return false;
    curT = nt;
    curChecks = nc;
    currentDirty = false;
    return true;
  };

  struct Group {
    camada::SMTExprRef expr;
    SmallVector<size_t, 1> indices;
  };

  for (bool unwinding : {false, true}) {
    auto reportFirst = [&](size_t idx, bool callViolated) {
      v.status[idx] = Status::Fails;
      v.kind = Verdict::Violated;
      v.check = &checks[idx];
      for (size_t j = idx + 1; j < checks.size(); ++j)
        if (checks[j].isUnwinding() == unwinding)
          v.status[j] = Status::NotChecked;
      if (callViolated)
        violated(*curT, *curChecks, idx);
    };

    std::vector<size_t> pending;
    for (size_t i = 0; i < checks.size(); ++i) {
      if (checks[i].isUnwinding() != unwinding)
        continue;
      if (curT->isFalse((*curChecks)[i].violated)) {
        v.status[i] = Status::Holds;
        continue;
      }
      if (unwinding && skipUnwindModel && curT->isTrue((*curChecks)[i].violated)) {
        if (!all) {
          reportFirst(i, /*callViolated=*/false);
          return v;
        }
        v.status[i] = Status::Fails;
        if (!v.check) {
          v.kind = Verdict::Violated;
          v.check = &checks[i];
        }
        continue;
      }
      pending.push_back(i);
    }

    // A violation already found stands; an unknown answer only leaves the
    // remaining checks unchecked, as in decide().
    auto unknown = [&](size_t idx, std::string detail) {
      if (!v.check) {
        v.kind = Verdict::Unknown;
        v.check = &checks[idx];
        v.detail = std::move(detail);
      }
      return v;
    };

    while (!pending.empty()) {
      if (!ensureFresh())
        return unknown(pending[0], "failed to create fresh solver");
      camada::SMTSolver &s = curT->solver();
      std::vector<Group> groups;
      DenseMap<const void *, size_t> byId;
      for (size_t idx : pending) {
        const checks::Check &check = (*curChecks)[idx];
        auto [it, fresh] = byId.try_emplace(check.violated.id(), groups.size());
        if (fresh)
          groups.push_back({curT->solverTerm(check.violated), {idx}});
        else
          groups[it->second].indices.push_back(idx);
      }
      auto exprs = [](ArrayRef<Group> gs) {
        std::vector<camada::SMTExprRef> es;
        for (const Group &g : gs)
          es.push_back(g.expr);
        return es;
      };
      auto violatedInModel = [&](const Group &g) {
        auto b = s.getBool(g.expr);
        return b && b.value();
      };
      // The first of gs, whose disjunction was satisfied, the model violates;
      // a lone one needs no evaluating.
      auto firstViolated = [&](ArrayRef<Group> gs) {
        return gs.size() == 1 ? gs.begin() : llvm::find_if(gs, violatedInModel);
      };
      s.addConstraint(disjunction(s, exprs(groups)));
      currentDirty = true;
      if (curT == &t)
        solverUsed = true;

      switch (encode::checked(s, s.check())) {
      case camada::CheckResult::UNSAT:
        for (size_t idx : pending)
          v.status[idx] = Status::Holds;
        pending.clear();
        break;
      case camada::CheckResult::SAT: {
        if (!all) {
          ArrayRef<Group> phase(groups);
          const Group *first = firstViolated(phase);
          if (first == phase.end())
            return unknown(pending[0], "the solver's model violates no check");
          // Report the lowest failing check, as decide() does. Groups are
          // in index order, so while one before first may fail too, ask for
          // one of those: their disjunction implies the one asserted, so it
          // only narrows the query. Where none can (or the solver cannot
          // tell), drop that scope and ask again for a model violating first.
          while (first != phase.begin()) {
            ArrayRef<Group> lower = phase.take_front(first - phase.begin());
            s.push();
            s.addConstraint(disjunction(s, exprs(lower)));
            camada::CheckResult earlierResult = encode::checked(s, s.check());
            if (earlierResult != camada::CheckResult::SAT) {
              // Unsatisfiable: the disjunction already asserted covers lower,
              // so none of those groups can fail anywhere; unknown says nothing.
              if (earlierResult == camada::CheckResult::UNSAT)
                for (const Group &g : lower)
                  for (size_t idx : g.indices)
                    v.status[idx] = Status::Holds;
              s.pop();
              s.addConstraint(first->expr);
              if (encode::checked(s, s.check()) != camada::CheckResult::SAT)
                return unknown(first->indices[0],
                               "solver answered unknown: " + s.reasonUnknownDetail());
              break;
            }
            const Group *earlier = firstViolated(lower);
            if (earlier == lower.end())
              return unknown(pending[0], "the solver's model violates no check");
            first = earlier;
          }
          reportFirst(first->indices[0], /*callViolated=*/true);
          return v;
        }
        std::vector<size_t> failed, next;
        for (const Group &g : groups) {
          std::vector<size_t> &to = groups.size() == 1 || violatedInModel(g) ? failed : next;
          to.insert(to.end(), g.indices.begin(), g.indices.end());
        }
        if (failed.empty())
          return unknown(pending[0], "the solver's model violates no check");
        std::sort(failed.begin(), failed.end());
        for (size_t idx : failed) {
          v.status[idx] = Status::Fails;
          if (!v.check) {
            v.kind = Verdict::Violated;
            v.check = &checks[idx];
          }
          violated(*curT, *curChecks, idx);
        }
        pending = std::move(next);
        break;
      }
      case camada::CheckResult::UNKNOWN:
        return unknown(pending[0], "solver answered unknown: " + s.reasonUnknownDetail());
      }
    }
  }
  return v;
}

} // namespace vereda::solver
