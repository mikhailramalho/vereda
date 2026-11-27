// Stage driver: parses options, runs the stages, prints the verdict.
#include "checks/checks.h"
#include "encode/encoder.h"
#include "ir/load.h"
#include "solver/solver.h"
#include "util/log.h"
#include "walker/walker.h"
#include "witness/counterexample.h"
#include "witness/witness.h"
#include "vereda-version.h"

#include <llvm/Config/llvm-config.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/raw_ostream.h>

#include <cstdio>
#include <cstdlib>
#include <new>

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
static cl::opt<bool> noUnwindingAssertions(
    "no-unwinding-assertions", cl::desc("Assume the unwinding bound instead of checking it"));
static cl::opt<bool> noSummarizeArrayLoops(
    "no-summarize-array-loops",
    cl::desc("Unroll the counting loops that only write a[i], check or count, instead of "
             "replacing them by their effect"));
static cl::opt<bool> forceMallocSuccess("force-malloc-success",
                                        cl::desc("malloc never returns null"));
static cl::opt<bool> noPointerCheck(
    "no-pointer-check", cl::desc("Do not check writes into string literals and const objects"));
static cl::opt<bool> noAssertions("no-assertions", cl::desc("Do not check user assertions"));
static cl::opt<bool> noDivByZeroCheck(
    "no-div-by-zero-check", cl::desc("Do not check integer division or remainder by zero"));
static cl::opt<bool> ubCountZeroCheck(
    "ub-count-zero-check", cl::desc("Check undefined zero arguments to count-leading/trailing-zeros"));
static cl::opt<bool> ubFloatCastOverflowCheck(
    "ub-float-cast-overflow-check",
    cl::desc("Check out-of-range floating-point to integer conversions"));
static cl::opt<bool> ubAlignmentCheck(
    "ub-alignment-check", cl::desc("Check alignment of typed memory accesses"));
static cl::opt<bool> ubUninitCheck(
    "ub-uninit-check", cl::desc("Check reads of uninitialised local values and memory"));
static cl::opt<bool> memoryLeakCheck(
    "memory-leak-check", cl::desc("Check for memory leaks (not implemented)"));
static cl::opt<bool> dataRaceCheck(
    "data-race-check", cl::desc("Check for data races (not implemented)"));
static cl::alias dataRacesCheck("data-races-check", cl::aliasopt(dataRaceCheck));
static cl::opt<bool> deadlockCheck(
    "deadlock-check", cl::desc("Check for deadlocks (not implemented)"));
static cl::opt<bool> noVLASizeCheck(
    "no-vla-size-check", cl::desc("Do not report stack allocations above PTRDIFF_MAX"));
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

using vereda::walker::Property;
static cl::opt<Property> property(
    "property", cl::desc("Count only the failures of one SV-COMP property, and print its verdict"),
    cl::values(clEnumValN(Property::All, "all", "every check (default)"),
               clEnumValN(Property::UnreachCall, "unreach-call",
                          "a call to reach_error, whose body is not walked"),
               clEnumValN(Property::NoOverflow, "no-overflow", "a clang sanitizer check")),
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
                                     !noPointerCheck,
                                     !noVLASizeCheck,
                                     !noAssertions,
                                     multiProperty,
                                     checkUninit,
                                     boundedLimit};
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
          ? vereda::solver::decide(terms, checks, multiProperty,
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
                                                     !noPointerCheck,
                                                     !noVLASizeCheck,
                                                     !noAssertions,
                                                     multiProperty,
                                                     checkUninit,
                                                     boundedLimit};
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
    for (size_t i = 0; i < checks.size(); ++i) {
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
    // A failed unwinding assertion is no violation of the property.
    svcomp(!bug                                  ? "UNKNOWN"
           : property == Property::UnreachCall ? "FALSE_REACH"
                                               : "FALSE_OVERFLOW");
    return finish(1);
  }
  WithColor(outs(), raw_ostream::GREEN, /*Bold=*/true) << "VERIFICATION SUCCESSFUL";
  outs() << "\n";
  svcomp("TRUE");
  return finish(0);
}

int main(int argc, char **argv) {
  // Out of memory ends the run. A std::bad_alloc thrown in the solver would
  // not: camada turns it into an unknown answer, and the next query runs on a
  // solver left half updated and crashes.
  std::set_new_handler([] {
    outs().flush();
    std::fputs("vereda: out of memory\n", stderr);
    std::_Exit(ErrorExit);
  });
  cl::SetVersionPrinter(printVersion);
  // With an error stream, a bad option returns false instead of exiting with 1.
  if (!cl::ParseCommandLineOptions(argc, argv, "Vereda: LLVM-IR verifier\n", &errs()))
    return ErrorExit;
  for (auto [enabled, flag] : {std::pair{bool(memoryLeakCheck), "--memory-leak-check"},
                              {bool(dataRaceCheck), "--data-race-check"},
                              {bool(deadlockCheck), "--deadlock-check"}}) {
    if (enabled) {
      errs() << "vereda: " << flag << " is not implemented\n";
      return ErrorExit;
    }
  }
  if (noAssertions && property == Property::UnreachCall) {
    errs() << "vereda: --no-assertions conflicts with --property unreach-call\n";
    return ErrorExit;
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
  Expected<std::unique_ptr<Module>> module =
      vereda::ir::load(inputFile, ctx, sys::path::parent_path(self), entry,
                       !noDivByZeroCheck && property == Property::All,
                       ubCountZeroCheck && property == Property::All,
                       ubFloatCastOverflowCheck && property == Property::All,
                       ubAlignmentCheck && property == Property::All,
                       ubUninitCheck && property == Property::All);
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
  Function *fn = (*module)->getFunction(entry);
  if (!fn || fn->isDeclaration()) {
    errs() << argv[0] << ": no function " << entry << "\n";
    return ErrorExit;
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
  if (unwind || noUnwindingAssertions) {
    errs() << argv[0] << ": --incremental-bmc chooses the bound and checks it\n";
    return ErrorExit;
  }
  // While every path runs a loop of known trip count past bound k, the
  // bounds below the one that unwinds it only add iterations of that loop,
  // and a bug one of them finds, every larger bound finds too. Those bounds
  // are skipped, but the bound at most doubles, so that a bug in an early
  // iteration of a long loop does not wait for a walk of all of it. The
  // largest bound is never skipped.
  unsigned unwindsAt = 0, walked = 0;
  std::unique_ptr<BMCState> retained;
  for (unsigned k = 1; unlimitedKSteps || k <= maxKStep; k += kStep) {
    if ((unlimitedKSteps || k + kStep <= maxKStep) && k < std::min(unwindsAt, 2 * walked))
      continue;
    walked = k;
    // Extend a prefix stopped in an unconditional outer loop. Otherwise
    // start afresh: extending a loop changes the state at its merged exits
    // and everything walked after them.
    progress("Checking bound k = " + std::to_string(k));
    if (int r = bmc(*fn, k, witnessInfoPtr, nullptr, &unwindsAt, &retained); r != BoundTooSmall)
      return r;
  }
  return finish(unknown("no verdict up to bound " + std::to_string(maxKStep)));
}
