// A Bitwuzla abort throws vereda::Unsupported with Bitwuzla's message instead
// of ending the process, and one inside a check, which camada turns into an
// unknown answer, is thrown again rather than leaving the solver in use
// (#412). The abort here is a real one: an option given a mode it lacks.
#undef NDEBUG
#include "solver/solver.h"
#include <camada/solvers/bitwuzlasolver.h>
#include <cassert>
#include <string>

using namespace vereda;

namespace {
// Makes Bitwuzla abort, as a bad option value does.
void abortBitwuzla() {
  BitwuzlaOptions *o = bitwuzla_options_new();
  bitwuzla_set_option_mode(o, BITWUZLA_OPT_BV_CNF_SCOPE, "no-such-mode");
}

// The message of the Unsupported that f throws, or "" if it throws none.
template <typename F> std::string thrown(F f) {
  try {
    f();
  } catch (const Unsupported &e) {
    return e.what();
  }
  return "";
}

// Bitwuzla whose checks abort inside the backend call.
struct Aborting : camada::BitwuzlaSolver {
  camada::CheckResult checkSatAssumingImpl(const std::vector<camada::SMTExprRef> &) override {
    abortBitwuzla();
    return camada::CheckResult::SAT;
  }
};
} // namespace

int main() {
  for (bool incremental : {false, true}) {
    auto s = solver::create(camada::SolverKind::Bitwuzla, incremental);
    assert(s);
    std::string msg = thrown(abortBitwuzla);
    assert(llvm::StringRef(msg).starts_with(encode::SolverAborted));
    assert(llvm::StringRef(msg).ends_with("invalid mode for option")); // Bitwuzla's, trimmed
  }

  // camada installs its own handler with every Bitwuzla context.
  camada::SMTSolverRef s(new Aborting());
  solver::throwOnAbort();
  encode::Terms t(*s);
  encode::Term x = t.symbol("x", s->mkBoolSort());
  std::string msg = thrown([&] { encode::checked(*s, s->checkSatAssuming({t.solverTerm(x)})); });
  assert(llvm::StringRef(msg).starts_with(encode::SolverAborted));

  checks::Checks checks;
  checks.push_back({checks::Property::Assertion, {}, "", "", 1, x});
  msg = thrown([&] { solver::decide(t, checks, false, false, [](const checks::Check &) {}); });
  assert(llvm::StringRef(msg).starts_with(encode::SolverAborted));
  return 0;
}
