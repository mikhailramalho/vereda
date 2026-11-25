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

Verdict decide(encode::Terms &t, const checks::Checks &checks, bool all,
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
        if ((checks[j].property != checks::Property::Assertion) == unwinding)
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
      auto [it, fresh] = byId.try_emplace(check.violated.id(), groups.size());
      if (fresh)
        groups.push_back({t.solverTerm(check.violated), {i}});
      else
        groups[it->second].indices.push_back(i);
    }

    auto checkIndividuallyPhase = [&](ArrayRef<Group> rem) -> bool {
      for (const Group &g : rem) {
        switch (s.checkSatAssuming({g.expr})) {
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
      switch (s.checkSatAssuming({disjunction(s, exprs)})) {
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

    while (!pending.empty()) {
      if (!ensureFresh()) {
        v.kind = Verdict::Unknown;
        v.check = &checks[pending[0]];
        v.detail = "failed to create fresh solver";
        return v;
      }
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
      std::vector<camada::SMTExprRef> exprs;
      exprs.reserve(groups.size());
      for (const Group &g : groups)
        exprs.push_back(g.expr);
      s.addConstraint(disjunction(s, exprs));
      currentDirty = true;
      if (curT == &t)
        solverUsed = true;

      switch (s.check()) {
      case camada::CheckResult::UNSAT:
        for (size_t idx : pending)
          v.status[idx] = Status::Holds;
        pending.clear();
        break;
      case camada::CheckResult::SAT: {
        std::vector<size_t> failed, next;
        if (groups.size() == 1) {
          failed.assign(groups[0].indices.begin(), groups[0].indices.end());
        } else {
          for (const Group &g : groups) {
            if (auto b = s.getBool(g.expr); b && b.value())
              failed.insert(failed.end(), g.indices.begin(), g.indices.end());
            else
              next.insert(next.end(), g.indices.begin(), g.indices.end());
          }
          if (failed.empty()) {
            // Defensive fallback if the solver did not evaluate any disjunct.
            failed.push_back(pending[0]);
            next.assign(pending.begin() + 1, pending.end());
          }
        }
        std::sort(failed.begin(), failed.end());
        if (!all) {
          reportFirst(failed[0], /*callViolated=*/true);
          return v;
        }
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
        v.kind = Verdict::Unknown;
        v.check = &checks[pending[0]];
        v.detail = "solver answered unknown: " + s.reasonUnknownDetail();
        return v;
      }
    }
  }
  return v;
}

} // namespace vereda::solver
