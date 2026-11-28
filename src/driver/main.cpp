// Stage driver: parses options, runs the stages, prints the verdict.
#include "analysis/termination.h"
#include "checks/checks.h"
#include "encode/encoder.h"
#include "ir/load.h"
#include "solver/solver.h"
#include "util/log.h"
#include "walker/walker.h"
#include "witness/counterexample.h"
#include "witness/witness.h"
#include "vereda-version.h"

#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/StringSwitch.h>
#include <llvm/Config/llvm-config.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/ErrorHandling.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/raw_ostream.h>

#include <cstdio>
#include <cstdlib>
#include <new>
#include <cerrno>
#include <map>
#include <numeric>
#include <set>

#ifdef VEREDA_MIMALLOC
#include <mimalloc.h>
#endif

using namespace llvm;
using vereda::util::progress;
using vereda::util::info;
using vereda::util::results;

static cl::opt<std::string> inputFile(cl::Positional, cl::desc("<file.bc|file.ll>"),
                                      cl::Required);
static cl::opt<bool> noBanner("no-banner", cl::Hidden,
                             cl::desc("Banner already printed by the front end"));
static cl::opt<bool> dumpIROnly("dump-ir-only", cl::desc("Print the IR after the pass list and exit"));
static cl::opt<bool> dumpIRToo("dump-ir-too", cl::desc("Print the IR after the pass list, then verify"));
static cl::opt<bool> dumpSMTOnly("dump-smt-only",
                                 cl::desc("Print the formula as SMT-LIB after the walk and exit; "
                                          "it is satisfiable exactly when some check fails"));
static cl::opt<bool> dumpSMTToo("dump-smt-too",
                                cl::desc("Print the formula as SMT-LIB after the walk, then solve"));
static cl::opt<std::string> dumpDir("dump-dir",
                                    cl::desc("Write the --dump-* output to files in this directory "
                                             "(ir.ll, formula.smt2) instead of printing it"));
static cl::opt<std::string> entry("entry", cl::desc("Entry function"), cl::init("main"));
static cl::opt<unsigned> unwind("unwind",
                                cl::desc("Loop iterations and recursion depth (default: "
                                         "unroll until the next iteration is unreachable)"),
                                cl::init(0));
static cl::opt<std::string> unwindSetName(
    "unwindsetname", cl::value_desc("name:idx:nr,..."),
    cl::desc("Unwind loop idx (from 0 in each function, as the unwinding assertions number "
             "them) of function name nr times instead of --unwind; name may be clang's USR "
             "(c:@F@name#)"));
// --unwindsetname's bounds by function and loop number.
static std::map<std::pair<std::string, unsigned>, unsigned> loopBounds;
static cl::opt<bool> noUnwindingAssertions(
    "no-unwinding-assertions", cl::desc("Assume the unwinding bound instead of checking it"));
static cl::opt<bool> noSummarizeArrayLoops(
    "no-summarize-array-loops",
    cl::desc("Unroll the counting loops that only write a[i], check or count, instead of "
             "replacing them by their effect"));
static cl::opt<bool> forceMallocSuccess("force-malloc-success",
                                        cl::desc("malloc never returns null"));
static cl::opt<bool> ubPointerCheck(
    "ub-pointer-check",
    cl::desc("Check writes into string literals and const objects, and frees (default on; "
             "=false disables)"),
    cl::init(true));
static cl::opt<bool> noAssertions("no-assertions", cl::desc("Do not check user assertions"));
static cl::opt<std::string> errorLabel("error-label", cl::value_desc("label"),
                                       cl::desc("Check that the C label <label> is never reached"));
static cl::opt<bool> ubDivByZeroCheck(
    "ub-div-by-zero-check",
    cl::desc("Check integer division and remainder by zero (default on; =false disables)"),
    cl::init(true));
static cl::opt<bool> ubCountZeroCheck(
    "ub-count-zero-check", cl::desc("Check undefined zero arguments to count-leading/trailing-zeros"));
static cl::opt<bool> ubFloatCastOverflowCheck(
    "ub-float-cast-overflow-check",
    cl::desc("Check out-of-range floating-point to integer conversions"));
static cl::opt<bool> ubAlignmentCheck(
    "ub-alignment-check", cl::desc("Check alignment of typed memory accesses"));
static cl::opt<bool> ubUninitCheck(
    "ub-uninit-check", cl::desc("Check reads of uninitialised local values and memory"));
static cl::opt<bool> ubSignedOverflowCheck(
    "ub-signed-overflow-check",
    cl::desc("Check the signed overflow traps compiled into the IR (default on; =false leaves "
             "them out; vereda.py compiles them in for C)"),
    cl::init(true));
static cl::opt<bool> unsignedOverflowCheck(
    "unsigned-overflow-check",
    cl::desc("Check the unsigned overflow traps compiled into the IR (default on; =false leaves "
             "them out; vereda.py compiles them in for C)"),
    cl::init(true));
static cl::opt<bool> ubShiftCheck(
    "ub-shift-check",
    cl::desc("Check the shift traps compiled into the IR (default on; =false leaves them out; "
             "vereda.py compiles them in for C)"),
    cl::init(true));
static cl::opt<bool> overflowCheck(
    "overflow-check",
    cl::desc("Both overflow flags; of it and either one's own flag, the later decides"),
    cl::init(true));
static cl::opt<bool> memoryLeakCheck(
    "memory-leak-check", cl::desc("Check for memory leaks (not implemented)"));
static cl::opt<bool> ubDataRaceCheck(
    "ub-data-race-check", cl::desc("Check for data races (not implemented)"));
static cl::opt<bool> deadlockCheck(
    "deadlock-check", cl::desc("Check for deadlocks (not implemented)"));
static cl::opt<bool> vlaSizeCheck(
    "vla-size-check",
    cl::desc("Check stack allocations above PTRDIFF_MAX (default on; =false disables)"),
    cl::init(true));
static cl::opt<bool> incrementalBMC(
    "incremental-bmc",
    cl::desc("Check bounds 1, 2, ... until the verdict does not depend on the bound"));
static cl::opt<unsigned> maxKStep("max-k-step", cl::desc("Largest bound for --incremental-bmc"),
                                  cl::init(50));
static cl::opt<bool> unlimitedKSteps("unlimited-k-steps",
                                     cl::desc("No largest bound for --incremental-bmc: raise it "
                                              "until a verdict or the time limit"));
static cl::opt<unsigned> kStep("k-step", cl::desc("Raise the bound of --incremental-bmc by this"),
                               cl::init(1));
static cl::opt<bool> multiProperty("multi-property",
                                   cl::desc("Check every property instead of stopping at the "
                                            "first violation"));
static cl::opt<bool> incrementalSolver(
    "incremental-solver",
    cl::desc("Keep one incremental solver across checks (with bv-cnf-scope=permanent on Bitwuzla) "
             "instead of fresh non-incremental solves"));
static cl::opt<bool> showLibraryStates(
    "show-library-states", cl::desc("Show Rust standard-library states in counterexamples"));
static cl::opt<bool> fp2bv("fp2bv",
                           cl::desc("Encode floating point in bit-vectors, as camada does, instead "
                                    "of the solver's floating-point theory"));
static cl::opt<std::string> witnessFile(
    "witness", cl::desc("Write the first counterexample that reaches reach_error to this file "
                        "as an SV-COMP violation witness (YAML, format 2.0); - is standard "
                        "output"),
    cl::value_desc("file"));
static cl::list<std::string> sanitized(
    "sanitized", cl::Hidden, cl::CommaSeparated,
    cl::desc("The clang sanitizer checks vereda.py compiled in, as -fsanitize= names them, "
             "for the [INFO] Checks line, which also lists those whose traps the IR holds"));

using vereda::walker::Property;
static cl::opt<Property> property(
    "property", cl::desc("Count only the failures of one SV-COMP property, and print its verdict"),
    cl::values(clEnumValN(Property::All, "all", "every check (default)"),
               clEnumValN(Property::UnreachCall, "unreach-call",
                          "a call to reach_error, whose body is not walked"),
               clEnumValN(Property::NoOverflow, "no-overflow", "a clang sanitizer check"),
               clEnumValN(Property::ValidFree, "valid-free",
                          "a free of a pointer no allocator returned, or already freed"),
               clEnumValN(Property::Termination, "termination",
                          "every run ends: TRUE when SCEV bounds every loop and nothing "
                          "recurses, or when the bound unwinds every loop; never FALSE")),
    cl::init(Property::All));
static cl::opt<camada::SolverKind> solverKind(
    "solver", cl::desc("SMT solver"),
    cl::values(clEnumValN(camada::SolverKind::Bitwuzla, "bitwuzla", "Bitwuzla (default)"),
               clEnumValN(camada::SolverKind::Z3, "z3", "Z3"),
               clEnumValN(camada::SolverKind::CVC5, "cvc5", "cvc5")),
    cl::init(camada::SolverKind::Bitwuzla));

static void printVersion(raw_ostream &os) {
  os << "Vereda " VEREDA_VERSION " (LLVM " LLVM_VERSION_STRING ", camada " VEREDA_CAMADA_VERSION
        ", commit " VEREDA_COMMIT ")\n";
  os.flush();
}

// Prints a dump, or writes it to name in --dump-dir. Returns false on a
// write error, already reported.
static bool dump(StringRef name, function_ref<void(raw_ostream &)> write) {
  if (dumpDir.empty()) {
    write(outs());
    return true;
  }
  SmallString<128> path(dumpDir);
  sys::path::append(path, name);
  std::error_code ec = sys::fs::create_directories(dumpDir);
  if (!ec) {
    raw_fd_ostream os(path, ec);
    if (!ec)
      write(os);
  }
  if (ec) {
    errs() << "vereda: cannot write " << path << ": " << ec.message() << "\n";
    return false;
  }
  info() << "Wrote " << path << "\n";
  return true;
}

// Under --property, the SV-COMP verdict on a line of its own, as ESBMC's
// wrapper prints it.
static void svcomp(StringRef verdict) {
  if (property != Property::All)
    outs() << verdict << "\n";
}

// Exit code of a run that ends in an error rather than a verdict: a bad
// option, an input that cannot be loaded, a solver that cannot be created,
// a dump that cannot be written or no memory left. ESBMC uses 6 too, so 1
// means VERIFICATION FAILED only.
static const int ErrorExit = 6;

[[noreturn]] static void outOfMemory() {
  outs().flush();
  std::fputs("vereda: out of memory\n", stderr);
  std::_Exit(ErrorExit);
}

static int unknown(const Twine &reason) {
  results();
  outs() << "\nVERIFICATION UNKNOWN: " << reason << "\n";
  svcomp("UNKNOWN");
  return 2;
}

// Ends the run with code once its output is written, without freeing the
// solver, the terms or the module: that took about 2 s after a large walk,
// for memory the process is about to give back
// (docs/survey/2026-11-24-exit-without-teardown.md). The files this driver
// writes are closed by then. A build with AddressSanitizer tears down as
// usual, so that its leak check still runs, and so does a run whose standard
// output failed, for outs()'s destructor to report it.
static int finish(int code) {
  outs().flush();
  errs().flush();
  if (!LLVM_ADDRESS_SANITIZER_BUILD && !outs().has_error()) {
    std::fflush(nullptr); // anything a solver printed through stdio
    std::_Exit(code);
  }
  return code;
}

// What bmc() returns under --incremental-bmc when a loop or recursion needs
// a larger bound.
static const int BoundTooSmall = -1;

// Whether the sanitizer traps of a kind are checked: own, or both when
// that came later; true unless the deciding flag is =false.
static bool trapsChecked(const cl::opt<bool> &own, const cl::opt<bool> *both = nullptr) {
  if (both && both->getNumOccurrences() &&
      (!own.getNumOccurrences() || both->getPosition() > own.getPosition()))
    return *both;
  return own;
}

// The checks this run counts, each named after the flag that controls it,
// for an [INFO] line that gives the verdict its scope. A property counts
// only its own checks and the unwinding assertions (walker::counts). The
// sanitizers are those vereda.py compiled in (--sanitized) and those whose
// traps the IR holds.
static std::string checkFamilies(const Module &m) {
  bool all = property == Property::All;
  SmallVector<StringRef, 12> on;
  if (all && !noAssertions)
    on.push_back("assertions");
  if (property == Property::UnreachCall)
    on.push_back("reach-error");
  // As the walker counts an error label: as an assertion, or the error
  // location under unreach-call.
  std::string label = "error-label " + errorLabel;
  if (!errorLabel.empty() && ((all && !noAssertions) || property == Property::UnreachCall))
    on.push_back(label);
  if (!noUnwindingAssertions)
    on.push_back("unwinding-assertions");
  if (all && ubPointerCheck)
    on.push_back("pointer");
  if (property == Property::ValidFree)
    on.push_back("valid-free");
  if (all && ubDivByZeroCheck)
    on.push_back("div-by-zero");
  if (all && vlaSizeCheck)
    on.push_back("vla-size");
  std::set<std::string> sanitizers(sanitized.begin(), sanitized.end());
  if (const NamedMDNode *md = m.getNamedMetadata("vereda.sanitized"))
    for (const MDNode *n : md->operands())
      sanitizers.insert(cast<MDString>(n->getOperand(0))->getString().str());
  if (all || property == Property::NoOverflow)
    for (const std::string &s : sanitizers) {
      StringRef name = StringSwitch<StringRef>(s)
                           .Case("signed-integer-overflow", "signed-overflow")
                           .Case("unsigned-integer-overflow", "unsigned-overflow")
                           .Case("integer-divide-by-zero", "div-by-zero")
                           .Default(s);
      if (!is_contained(on, name))
        on.push_back(name);
    }
  if (all && ubFloatCastOverflowCheck)
    on.push_back("float-cast-overflow");
  if (all && ubCountZeroCheck)
    on.push_back("count-zero");
  if (all && ubAlignmentCheck)
    on.push_back("alignment");
  if (all && ubUninitCheck)
    on.push_back("uninit");
  return on.empty() ? "none" : join(on, ", ");
}

struct BMCState {
  camada::SMTSolverRef solver;
  vereda::encode::Terms terms;
  vereda::checks::Checks checks;
  vereda::walker::Trace trace;
  std::unique_ptr<vereda::memory::Memory> memory;
  std::unique_ptr<vereda::encode::Encoder> encoder;
  std::unique_ptr<vereda::walker::Incremental> walk;

  BMCState(camada::SMTSolverRef solver, Function &fn, bool trackInit = false)
      : solver(std::move(solver)), terms(*this->solver),
        memory(std::make_unique<vereda::memory::Memory>(terms, fn.getParent()->getDataLayout(),
                                                        trackInit)),
        encoder(std::make_unique<vereda::encode::Encoder>(terms, *memory, fp2bv)) {}

  bool resumable() const { return walk && walk->resumable(); }
};

// Walks fn with the given bound, solves and prints the verdict; returns the
// exit code. Under --incremental-bmc, a failed unwinding or recursion
// assertion is not a verdict but BoundTooSmall, with unwindsAt set to its
// (checks.h); an unknown from the solver still ends the run, as in ESBMC.
// When releaseModule is non-null (single-walk mode), the IR module is freed
// right after the walk, before solving, to keep peak memory down.
// retained keeps a resumable prefix and its solver alive between bounds.
static int bmc(Function &fn, unsigned bound, const vereda::witness::WitnessModuleInfo *witnessInfo,
               std::unique_ptr<Module> *releaseModule = nullptr, unsigned *unwindsAt = nullptr,
               std::unique_ptr<BMCState> *retained = nullptr) {
  std::unique_ptr<BMCState> local;
  auto &state = retained ? *retained : local;
  bool checkUninit = ubUninitCheck && property == Property::All;
  if (!state || !state->resumable()) {
    Expected<camada::SMTSolverRef> created =
        vereda::solver::create(solverKind, incrementalSolver || incrementalBMC);
    if (!created) {
      errs() << "vereda: " << toString(created.takeError()) << "\n";
      return ErrorExit;
    }
    try {
      state = std::make_unique<BMCState>(std::move(*created), fn, checkUninit);
    } catch (const vereda::Unsupported &e) {
      return unknown(Twine("unsupported: ") + e.what());
    }
  }
  camada::SMTSolver &solver = *state->solver;
  progress((state->resumable() ? "Resuming " : "Walking ") + entry);
  vereda::util::Stopwatch walkTime;
  auto &terms = state->terms;
  auto &checks = state->checks;
  auto &trace = state->trace;
  unsigned symbols = 0;
  constexpr unsigned MaxBoundedUnroll = 1024;
  unsigned boundedLimit = !incrementalBMC
                              ? (bound ? 0u : MaxBoundedUnroll)
                              : (unlimitedKSteps ? MaxBoundedUnroll
                                                 : (maxKStep > 1
                                                        ? std::min(MaxBoundedUnroll,
                                                                   maxKStep.getValue() - 1)
                                                        : 0u));
  {
    auto &enc = *state->encoder;
    auto &memory = *state->memory;
    vereda::walker::Options walkOpts{bound,
                                     !noUnwindingAssertions,
                                     forceMallocSuccess,
                                     property,
                                     !incrementalBMC,
                                     !noSummarizeArrayLoops,
                                     !witnessFile.empty(),
                                     ubPointerCheck,
                                     vlaSizeCheck,
                                     !noAssertions,
                                     multiProperty,
                                     checkUninit,
                                     boundedLimit,
                                     loopBounds};
    walkOpts.pragmaBounds = !incrementalBMC;
    if (retained && !state->walk)
      state->walk = std::make_unique<vereda::walker::Incremental>(fn, walkOpts, enc, memory,
                                                               checks, trace);
    if (Error e = state->walk ? state->walk->advance(bound)
                             : vereda::walker::walk(fn, walkOpts, enc, memory, checks, trace))
      return finish(unknown(toString(std::move(e))));
    symbols = enc.symbolCount();
  }
  if (!state->resumable()) {
    state->walk.reset();
    state->encoder.reset();
    state->memory.reset();
  }
  // Peak memory after the walk covers the walker and the terms it built;
  // the growth during solving is the solver's search.
  info() << "Walk time: " << walkTime.elapsed() << " (" << checks.size()
         << (checks.size() == 1 ? " check, " : " checks, ") << symbols
         << " symbols, peak memory " << vereda::util::peakMemory() << ")\n";
  outs().flush(); // solving may run until a timeout kills the process

  if (dumpSMTOnly || dumpSMTToo) {
    // The checks are assumptions in decide(); here their disjunction is
    // asserted, in a scope of its own, so the script stands alone.
    vereda::encode::Term anyViolated = terms.boolean(false);
    for (const vereda::checks::Check &c : checks)
      anyViolated = terms.lor(anyViolated, c.violated);
    solver.push();
    solver.addConstraint(terms.solverTerm(anyViolated));
    std::string formula;
    solver.dump(formula);
    solver.pop();
    std::string name = incrementalBMC ? "formula-k" + std::to_string(bound) + ".smt2" : "formula.smt2";
    if (!dump(name, [&](raw_ostream &os) { os << formula; }))
      return finish(ErrorExit);
    if (dumpSMTOnly)
      return finish(0);
  }

  if (!state->resumable())
    terms.releaseWalkTables();

  auto isAssertion = [](const vereda::checks::Check &c) {
    return !c.isUnwinding();
  };
  bool hasNonConstAssert = false, hasNonConstUnwind = false;
  for (const vereda::checks::Check &c : checks) {
    if (terms.isFalse(c.violated))
      continue;
    if (isAssertion(c))
      hasNonConstAssert = true;
    else if (!(incrementalBMC && terms.isTrue(c.violated)))
      hasNonConstUnwind = true;
  }
  bool useIncremental = incrementalSolver || state->resumable();
  bool mayRewalk = !useIncremental && (multiProperty || (hasNonConstAssert && hasNonConstUnwind));
  if (releaseModule && !mayRewalk)
    releaseModule->reset();

  progress("Solving with " + solver.getSolverNameAndVersion());
  vereda::util::Stopwatch solveTime;
  using vereda::solver::Status;
  // The time so far goes before each error trace, which is not solving
  // time, and after solving when there is no trace.
  bool traced = false, witnessWritten = false;
  auto solvingTime = [&] {
    std::string msg;
    raw_string_ostream(msg) << "Solving time: " << solveTime.elapsed() << " (peak memory "
                            << vereda::util::peakMemory() << ")";
    info() << msg << "\n";
  };
  auto onViolated = [&](vereda::encode::Terms &curTerms, const vereda::checks::Checks &curChecks,
                        const vereda::walker::Trace &curTrace, size_t idx) {
    const vereda::checks::Check &c = curChecks[idx];
    // Under --incremental-bmc an unwound loop asks for a larger bound;
    // it is not a bug to show.
    if (incrementalBMC && !isAssertion(c))
      return;
    solvingTime();
    traced = true;
    progress("Building error trace");
    // One reader for both, so the witness asks the solver for no value
    // the counterexample has read.
    vereda::solver::Model model(curTerms.solver(), curTerms);
    model.assumeTrue(c.violated);
    vereda::witness::printCounterexample(model, curTrace, curChecks, idx, outs(),
                                           showLibraryStates);
    if (witnessInfo && !witnessWritten) {
      outs().flush(); // the file may be standard output too
      Error err = vereda::witness::violationWitness(
          model, curTrace, curChecks, idx, *witnessInfo, entry,
          [&](function_ref<void(raw_ostream &)> write) {
            witnessWritten = true;
            std::error_code ec;
            raw_fd_ostream os(witnessFile, ec);
            if (ec)
              errs() << "vereda: cannot write " << witnessFile << ": " << ec.message() << "\n";
            else
              write(os);
          });
      if (err)
        errs() << "vereda: no witness: " << toString(std::move(err)) << "\n";
    }
  };
  std::unique_ptr<BMCState> freshState;
  bool initialSolverUsed = false;
  vereda::solver::Verdict verdict =
      useIncremental
          ? vereda::solver::decide(terms, checks, multiProperty, incrementalBMC,
                                   [&](const vereda::checks::Check &c) {
                                     onViolated(terms, checks, trace, &c - checks.data());
                                   })
          : vereda::solver::decideNonIncremental(
                terms, checks, multiProperty, incrementalBMC,
                [&]() -> std::pair<vereda::encode::Terms *, const vereda::checks::Checks *> {
                  freshState.reset();
                  Expected<camada::SMTSolverRef> created =
                      vereda::solver::create(solverKind, /*incremental=*/false);
                  if (!created) {
                    consumeError(created.takeError());
                    return {nullptr, nullptr};
                  }
                  try {
                    freshState = std::make_unique<BMCState>(std::move(*created), fn, checkUninit);
                  } catch (const vereda::Unsupported &) {
                    return {nullptr, nullptr};
                  }
                  vereda::walker::Options rewalkOpts{bound,
                                                     !noUnwindingAssertions,
                                                     forceMallocSuccess,
                                                     property,
                                                     /*showIterations=*/false,
                                                     !noSummarizeArrayLoops,
                                                     !witnessFile.empty(),
                                                     ubPointerCheck,
                                                     vlaSizeCheck,
                                                     !noAssertions,
                                                     multiProperty,
                                                     checkUninit,
                                                     boundedLimit,
                                                     loopBounds};
                  rewalkOpts.pragmaBounds = !incrementalBMC;
                  if (Error e = vereda::walker::walk(fn, rewalkOpts, *freshState->encoder,
                                                     *freshState->memory, freshState->checks,
                                                     freshState->trace)) {
                    consumeError(std::move(e));
                    return {nullptr, nullptr};
                  }
                  freshState->encoder.reset();
                  freshState->memory.reset();
                  freshState->terms.releaseWalkTables();
                  return {&freshState->terms, &freshState->checks};
                },
                [&](vereda::encode::Terms &curTerms, const vereda::checks::Checks &curChecks,
                    size_t idx) {
                  const vereda::walker::Trace &curTrace =
                      (&curTerms == &terms) ? trace : freshState->trace;
                  onViolated(curTerms, curChecks, curTrace, idx);
                },
                initialSolverUsed);
  if (!traced)
    solvingTime();

  if (verdict.kind == vereda::solver::Verdict::Unknown)
    return finish(unknown(verdict.detail));

  bool bug = false;
  for (size_t i = 0; i < checks.size(); ++i)
    bug |= verdict.status[i] == Status::Fails && isAssertion(checks[i]);
  if (incrementalBMC && verdict.check && !bug) {
    info() << "Bound " << bound << " too small: " << verdict.check->comment << "\n";
    *unwindsAt = verdict.check->unwindsAt;
    if (!incrementalSolver && initialSolverUsed)
      state.reset();
    return BoundTooSmall;
  }

  {
    results();
    std::string lastUnit;
    unsigned checked = 0, failed = 0;
    vereda::checks::number(checks);
    std::vector<size_t> order(checks.size());
    std::iota(order.begin(), order.end(), 0);
    std::stable_sort(order.begin(), order.end(), [&](size_t a, size_t b) {
      return vereda::checks::before(checks[a], checks[b]);
    });
    for (size_t i : order) {
      if (verdict.status[i] == Status::NotChecked)
        continue;
      const vereda::checks::Check &c = checks[i];
      bool fails = verdict.status[i] == Status::Fails;
      ++checked;
      failed += fails;
      std::string unit = c.loc.file + ", function " + llvm::demangle(c.loc.function);
      if (unit != lastUnit)
        outs() << (lastUnit.empty() ? "" : "\n") << unit << "\n";
      lastUnit = unit;
      outs() << "  ";
      WithColor(outs(), fails ? raw_ostream::RED : raw_ostream::GREEN, /*Bold=*/true)
          << (fails ? "FAILED" : "SUCCESS");
      outs() << (fails ? "       [" : "      [") << c.id() << "]  line " << c.loc.line << "  " << c.comment << "\n";
    }
    outs() << "\nProperties: " << checked << " checked, " << checked - failed
           << " passed, " << failed << " failed, " << checks.size() - checked << " not checked\n";
  }
  outs() << "\n";
  if (verdict.check) {
    WithColor(outs(), raw_ostream::RED, /*Bold=*/true) << "VERIFICATION FAILED";
    outs() << "\n";
    // A failed unwinding assertion is no violation of the property, nor any
    // failure of termination, which shows no non-termination.
    svcomp(!bug || property == Property::Termination ? "UNKNOWN"
           : property == Property::UnreachCall ? "FALSE_REACH"
           : property == Property::ValidFree   ? "FALSE_FREE"
                                               : "FALSE_OVERFLOW");
    return finish(1);
  }
  WithColor(outs(), raw_ostream::GREEN, /*Bold=*/true) << "VERIFICATION SUCCESSFUL";
  outs() << "\n";
  svcomp("TRUE");
  return finish(0);
}

static int run(int argc, char **argv) {
  cl::SetVersionPrinter(printVersion);
  // With an error stream, a bad option returns false instead of exiting with 1.
  if (!cl::ParseCommandLineOptions(argc, argv, "Vereda: LLVM-IR verifier\n", &errs()))
    return ErrorExit;
  for (auto [enabled, flag] : {std::pair{bool(memoryLeakCheck), "--memory-leak-check"},
                              {bool(ubDataRaceCheck), "--ub-data-race-check"},
                              {bool(deadlockCheck), "--deadlock-check"}}) {
    if (enabled) {
      errs() << "vereda: " << flag << " is not implemented\n";
      return ErrorExit;
    }
  }
  if (noUnwindingAssertions && property == Property::Termination) {
    errs() << "vereda: --no-unwinding-assertions conflicts with --property termination\n";
    return ErrorExit;
  }
  if (noAssertions && property == Property::UnreachCall) {
    errs() << "vereda: --no-assertions conflicts with --property unreach-call\n";
    return ErrorExit;
  }
  if (!ubPointerCheck && property == Property::ValidFree) {
    errs() << "vereda: --ub-pointer-check=false conflicts with --property valid-free\n";
    return ErrorExit;
  }
  SmallVector<StringRef, 4> sets;
  StringRef(unwindSetName).split(sets, ',', -1, false);
  for (StringRef set : sets) {
    auto [rest, nr] = set.rsplit(':');
    auto [name, idx] = rest.rsplit(':');
    unsigned number, bound;
    if (name.empty() || idx.getAsInteger(10, number) || nr.getAsInteger(10, bound)) {
      errs() << "vereda: --unwindsetname: bad entry " << set << "\n";
      return ErrorExit;
    }
    name = name.substr(name.rfind('@') + 1); // c:@F@name# or file.c@name
    name.consume_back("#");
    loopBounds[{name.str(), number}] = bound;
  }
  if (dumpDir.getNumOccurrences() && !dumpIROnly && !dumpIRToo && !dumpSMTOnly && !dumpSMTToo) {
    errs() << argv[0] << ": --dump-dir needs a --dump-* flag\n";
    return ErrorExit;
  }
  if (!noBanner && !((dumpIROnly || dumpSMTOnly) && dumpDir.empty()))
    printVersion(outs());

  progress("Loading " + inputFile);
  vereda::util::Stopwatch loadTime;
  LLVMContext ctx;
  // The library models sit next to the binary.
  std::string self = sys::fs::getMainExecutable(argv[0], reinterpret_cast<void *>(&printVersion));
  // The sanitizer traps on IR input that a flag turned off.
  std::set<std::string> unchecked;
  if (!trapsChecked(ubSignedOverflowCheck, &overflowCheck))
    unchecked.insert("signed-integer-overflow");
  if (!trapsChecked(unsignedOverflowCheck, &overflowCheck))
    unchecked.insert("unsigned-integer-overflow");
  if (!trapsChecked(ubShiftCheck))
    unchecked.insert("shift");
  if (!ubDivByZeroCheck)
    unchecked.insert("integer-divide-by-zero");
  Expected<std::unique_ptr<Module>> module =
      vereda::ir::load(inputFile, ctx, sys::path::parent_path(self), entry,
                       ubDivByZeroCheck && property == Property::All,
                       ubCountZeroCheck && property == Property::All,
                       ubFloatCastOverflowCheck && property == Property::All,
                       ubAlignmentCheck && property == Property::All,
                       ubUninitCheck && property == Property::All, errorLabel, unchecked);
  if (!module) {
    errs() << argv[0] << ": " << toString(module.takeError()) << "\n";
    return ErrorExit;
  }
  info() << "Target: " << (*module)->getDataLayout().getPointerSizeInBits() << "-bit "
         << ((*module)->getDataLayout().isLittleEndian() ? "little" : "big") << "-endian "
         << (*module)->getTargetTriple().str() << "\n";
  info() << "IR preparation time: " << loadTime.elapsed() << "\n";
  if (dumpIROnly || dumpIRToo) {
    if (!dump("ir.ll", [&](raw_ostream &os) { (*module)->print(os, nullptr); }))
      return ErrorExit;
    if (dumpIROnly)
      return 0;
  }
  info() << "Checks: " << checkFamilies(**module) << "\n";
  Function *fn = (*module)->getFunction(entry);
  if (!fn || fn->isDeclaration()) {
    errs() << argv[0] << ": no function " << entry << "\n";
    return ErrorExit;
  }

  if (property == Property::Termination) {
    auto [unbounded, unseen] = vereda::analysis::termination(*fn);
    if (!unseen.empty())
      return finish(unknown(unseen));
    if (unbounded.empty()) {
      results();
      outs() << "\nSCEV bounds every loop and no function recurses\n\n";
      WithColor(outs(), raw_ostream::GREEN, /*Bold=*/true) << "VERIFICATION SUCCESSFUL";
      outs() << "\n";
      svcomp("TRUE");
      return finish(0);
    }
    // The walk may still unwind every loop within its bound.
    info() << "Termination not shown without a walk: " << unbounded << "\n";
  }

  std::optional<vereda::witness::WitnessModuleInfo> witnessInfo;
  if (!witnessFile.empty())
    witnessInfo = vereda::witness::extractModuleInfo(**module);
  const vereda::witness::WitnessModuleInfo *witnessInfoPtr =
      witnessInfo ? &*witnessInfo : nullptr;

  if (!incrementalBMC) {
    cl::Option *boundFlags[] = {&maxKStep, &unlimitedKSteps, &kStep};
    for (cl::Option *o : boundFlags)
      if (o->getNumOccurrences()) {
        errs() << argv[0] << ": --" << o->ArgStr << " needs --incremental-bmc\n";
        return ErrorExit;
      }
    return bmc(*fn, unwind, witnessInfoPtr, &*module);
  }
  if (!kStep) {
    errs() << argv[0] << ": --k-step must be at least 1\n";
    return ErrorExit;
  }
  if (dumpSMTOnly) {
    errs() << argv[0] << ": --dump-smt-only stops before the solver picks the bound; use --unwind\n";
    return ErrorExit;
  }
  if (unwind || noUnwindingAssertions || !loopBounds.empty()) {
    errs() << argv[0] << ": --incremental-bmc chooses the bound and checks it\n";
    return ErrorExit;
  }
  // When bound k cuts a loop whose maximum trip count SCEV knows, the bounds
  // below the one that unwinds it only add iterations of that loop, and a bug
  // one of them finds, every larger bound finds too. Those bounds are skipped,
  // but the bound at most doubles, so that a bug in an early iteration of a
  // long loop does not wait for a walk of all of it. The largest bound is
  // never skipped.
  unsigned unwindsAt = 0, walked = 0;
  std::unique_ptr<BMCState> retained;
  for (unsigned k = 1; unlimitedKSteps || k <= maxKStep; k += kStep) {
    if ((unlimitedKSteps || k + kStep <= maxKStep) && k < std::min(unwindsAt, 2 * walked))
      continue;
    walked = k;
    // Extend the prefix stopped at the cut loop or recursive call when the walk
    // kept one (see walker::Incremental); otherwise start afresh.
    progress("Checking bound k = " + std::to_string(k));
    if (int r = bmc(*fn, k, witnessInfoPtr, nullptr, &unwindsAt, &retained); r != BoundTooSmall)
      return r;
  }
  return finish(unknown("no verdict up to bound " + std::to_string(maxKStep)));
}

int main(int argc, char **argv) {
  // Out of memory ends the run wherever an allocation fails. A std::bad_alloc
  // thrown in the solver would not: camada turns it into an unknown answer,
  // and the next query runs on a solver left half updated and crashes.
  // operator new calls the new-handler, but Bitwuzla allocates its nodes with
  // calloc and throws std::bad_alloc itself (#384); mimalloc's error hook sees
  // that failure first. In a build with glibc's malloc, the catch takes a
  // std::bad_alloc that escapes the solver, and encode::checked throws again
  // one that camada turned into an unknown answer (#399).
  std::set_new_handler(outOfMemory);
  // LLVM's own allocation checks (report_bad_alloc_error) print "LLVM ERROR:
  // out of memory" and abort unless a handler is installed (#480).
  install_bad_alloc_error_handler([](void *, const char *, bool) { outOfMemory(); });
#ifdef VEREDA_MIMALLOC
  mi_register_error([](int err, void *) {
    if (err == ENOMEM)
      outOfMemory();
  }, nullptr);
#endif
  try {
    return run(argc, argv);
  } catch (const std::bad_alloc &) {
    outOfMemory();
  } catch (const vereda::Unsupported &e) { // a solver abort (solver::throwOnAbort)
    return unknown(e.what());
  }
}
