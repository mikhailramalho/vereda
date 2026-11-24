// Stage driver: parses options, runs the stages, prints the verdict.
#include "checks/checks.h"
#include "encode/encoder.h"
#include "ir/load.h"
#include "solver/solver.h"
#include "util/log.h"
#include "walker/walker.h"
#include "witness/counterexample.h"
#include "witness/witness.h"

#include <llvm/Config/llvm-config.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/WithColor.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;
using vereda::util::progress;

static cl::opt<std::string> inputFile(cl::Positional, cl::desc("<file.bc|file.ll>"),
                                      cl::Required);
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
// ESBMC's flag for summaries of the loops that write no array, which Vereda
// makes by default; accepted so that ESBMC's command lines run.
static cl::opt<bool> summarizeArrayLoops("summarize-array-loops",
                                         cl::desc("The default; accepted for ESBMC"));
static cl::opt<bool> forceMallocSuccess("force-malloc-success",
                                        cl::desc("malloc never returns null"));
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
static cl::opt<std::string> solverName("solver", cl::desc("SMT solver: bitwuzla, z3, cvc5"),
                                       cl::init("bitwuzla"));

static void printVersion(raw_ostream &os) {
  os << "Vereda " VEREDA_VERSION " (LLVM " LLVM_VERSION_STRING ", camada " VEREDA_CAMADA_VERSION
        ")\n";
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
  outs() << "Wrote " << path << "\n";
  return true;
}

// Under --property, the SV-COMP verdict on a line of its own, as ESBMC's
// wrapper prints it.
static void svcomp(StringRef verdict) {
  if (property != Property::All)
    outs() << verdict << "\n";
}

static int unknown(const Twine &reason) {
  outs() << "\nVERIFICATION UNKNOWN: " << reason << "\n";
  svcomp("UNKNOWN");
  return 2;
}

// What bmc() returns under --incremental-bmc when a loop or recursion needs
// a larger bound.
static const int BoundTooSmall = -1;

// Walks fn with the given bound, solves and prints the verdict; returns the
// exit code. Under --incremental-bmc, a failed unwinding or recursion
// assertion is not a verdict but BoundTooSmall; an unknown from the solver
// still ends the run, as in ESBMC.
static int bmc(Function &fn, unsigned bound) {
  Expected<camada::SMTSolverRef> created = vereda::solver::create(solverName);
  if (!created) {
    errs() << "vereda: " << toString(created.takeError()) << "\n";
    return 1;
  }
  camada::SMTSolver &solver = **created;
  progress("Walking " + entry);
  vereda::util::Stopwatch walkTime;
  vereda::encode::Terms terms(solver);
  std::unique_ptr<vereda::memory::Memory> memory;
  try {
    memory = std::make_unique<vereda::memory::Memory>(terms, fn.getParent()->getDataLayout());
  } catch (const vereda::Unsupported &e) {
    return unknown(Twine("unsupported: ") + e.what());
  }
  vereda::encode::Encoder enc(terms, *memory, fp2bv);
  vereda::checks::Checks checks;
  vereda::walker::Trace trace;
  vereda::walker::Options walkOpts{bound, !noUnwindingAssertions, forceMallocSuccess, property,
                                   !incrementalBMC,
                                   !noSummarizeArrayLoops,
                                   !witnessFile.empty()};
  if (Error e = vereda::walker::walk(fn, walkOpts, enc, *memory, checks, trace))
    return unknown(toString(std::move(e)));
  // Peak memory after the walk covers the walker and the terms it built;
  // the growth during solving is the solver's search.
  outs() << "Walk time: " << walkTime.elapsed() << " (" << checks.size()
         << (checks.size() == 1 ? " check, " : " checks, ") << enc.symbolCount()
         << " symbols, peak memory " << vereda::util::peakMemory() << ")\n";
  outs().flush(); // solving may run until a timeout kills the process

  if (dumpSMTOnly || dumpSMTToo) {
    // The checks are assumptions in decide(); here their disjunction is
    // asserted, in a scope of its own, so the script stands alone.
    camada::SMTExprRef anyViolated = terms.boolean(false);
    for (const vereda::checks::Check &c : checks)
      anyViolated = terms.lor(anyViolated, c.violated);
    solver.push();
    solver.addConstraint(anyViolated);
    std::string formula;
    solver.dump(formula);
    solver.pop();
    std::string name = incrementalBMC ? "formula-k" + std::to_string(bound) + ".smt2" : "formula.smt2";
    if (!dump(name, [&](raw_ostream &os) { os << formula; }))
      return 1;
    if (dumpSMTOnly)
      return 0;
  }

  progress("Solving with " + solver.getSolverNameAndVersion());
  vereda::util::Stopwatch solveTime;
  using vereda::solver::Status;
  auto isAssertion = [](const vereda::checks::Check &c) {
    return c.property == vereda::checks::Property::Assertion;
  };
  // The time so far goes before each error trace, which is not solving
  // time, and after solving when there is no trace.
  bool traced = false, witnessWritten = false;
  auto solvingTime = [&] {
    std::string msg;
    raw_string_ostream(msg) << "Solving time: " << solveTime.elapsed() << " (peak memory "
                            << vereda::util::peakMemory() << ")";
    progress(msg);
  };
  vereda::solver::Verdict verdict =
      vereda::solver::decide(solver, checks, multiProperty, [&](const vereda::checks::Check &c) {
        // Under --incremental-bmc an unwound loop asks for a larger bound;
        // it is not a bug to show.
        if (incrementalBMC && !isAssertion(c))
          return;
        solvingTime();
        traced = true;
        progress("Building error trace");
        vereda::witness::printCounterexample(solver, trace, checks, &c - checks.data(), outs());
        if (!witnessFile.empty() && !witnessWritten) {
          Expected<std::string> witness = vereda::witness::violationWitness(
              solver, trace, checks, &c - checks.data(), *fn.getParent(), entry);
          outs().flush(); // the file may be standard output too
          if (!witness) {
            errs() << "vereda: no witness: " << toString(witness.takeError()) << "\n";
            return;
          }
          witnessWritten = true;
          std::error_code ec;
          raw_fd_ostream os(witnessFile, ec);
          if (ec)
            errs() << "vereda: cannot write " << witnessFile << ": " << ec.message() << "\n";
          else
            os << *witness;
        }
      });
  if (!traced)
    solvingTime();

  if (verdict.kind == vereda::solver::Verdict::Unknown)
    return unknown(verdict.detail);

  bool bug = false;
  for (size_t i = 0; i < checks.size(); ++i)
    bug |= verdict.status[i] == Status::Fails && isAssertion(checks[i]);
  if (incrementalBMC && verdict.check && !bug) {
    outs() << "Bound " << bound << " too small: " << verdict.check->comment << "\n";
    return BoundTooSmall;
  }

  if (!checks.empty()) {
    outs() << "\n** Results:\n";
    std::string lastUnit;
    unsigned checked = 0, failed = 0;
    for (size_t i = 0; i < checks.size(); ++i) {
      if (verdict.status[i] == Status::NotChecked)
        continue;
      const vereda::checks::Check &c = checks[i];
      bool fails = verdict.status[i] == Status::Fails;
      ++checked;
      failed += fails;
      std::string unit = c.loc.file + ", function " + c.loc.function;
      if (unit != lastUnit)
        outs() << (lastUnit.empty() ? "" : "\n") << unit << "\n";
      lastUnit = unit;
      outs() << "  ";
      WithColor(outs(), fails ? raw_ostream::RED : raw_ostream::GREEN, /*Bold=*/true)
          << (fails ? "FAILED" : "SUCCESS");
      outs() << (fails ? "       [" : "      [") << c.id() << "]  line " << c.loc.line << "  " << c.comment << "\n";
    }
    outs() << "\n** " << failed << " of " << checked << " properties failed\n";
  }
  outs() << "\n";
  if (verdict.check) {
    WithColor(outs(), raw_ostream::RED, /*Bold=*/true) << "VERIFICATION FAILED";
    outs() << "\n";
    // A failed unwinding assertion is no violation of the property.
    svcomp(!bug                                  ? "UNKNOWN"
           : property == Property::UnreachCall ? "FALSE_REACH"
                                               : "FALSE_OVERFLOW");
    return 1;
  }
  WithColor(outs(), raw_ostream::GREEN, /*Bold=*/true) << "VERIFICATION SUCCESSFUL";
  outs() << "\n";
  svcomp("TRUE");
  return 0;
}

int main(int argc, char **argv) {
  cl::SetVersionPrinter(printVersion);
  cl::ParseCommandLineOptions(argc, argv, "Vereda: LLVM-IR verifier\n");
  if (dumpDir.getNumOccurrences() && !dumpIROnly && !dumpIRToo && !dumpSMTOnly && !dumpSMTToo) {
    errs() << argv[0] << ": --dump-dir needs a --dump-* flag\n";
    return 1;
  }

  progress("Loading " + inputFile);
  vereda::util::Stopwatch loadTime;
  LLVMContext ctx;
  // The library models sit next to the binary.
  std::string self = sys::fs::getMainExecutable(argv[0], reinterpret_cast<void *>(&printVersion));
  Expected<std::unique_ptr<Module>> module =
      vereda::ir::load(inputFile, ctx, sys::path::parent_path(self));
  if (!module) {
    errs() << argv[0] << ": " << toString(module.takeError()) << "\n";
    return 1;
  }
  outs() << "Target: " << (*module)->getDataLayout().getPointerSizeInBits() << "-bit "
         << ((*module)->getDataLayout().isLittleEndian() ? "little" : "big") << "-endian "
         << (*module)->getTargetTriple().str() << "\n";
  outs() << "IR preparation time: " << loadTime.elapsed() << "\n";
  if (dumpIROnly || dumpIRToo) {
    if (!dump("ir.ll", [&](raw_ostream &os) { (*module)->print(os, nullptr); }))
      return 1;
    if (dumpIROnly)
      return 0;
  }
  Function *fn = (*module)->getFunction(entry);
  if (!fn || fn->isDeclaration()) {
    errs() << argv[0] << ": no function " << entry << "\n";
    return 1;
  }

  if (!incrementalBMC) {
    cl::Option *boundFlags[] = {&maxKStep, &unlimitedKSteps, &kStep};
    for (cl::Option *o : boundFlags)
      if (o->getNumOccurrences()) {
        errs() << argv[0] << ": --" << o->ArgStr << " needs --incremental-bmc\n";
        return 1;
      }
    return bmc(*fn, unwind);
  }
  if (!kStep) {
    errs() << argv[0] << ": --k-step must be at least 1\n";
    return 1;
  }
  if (dumpSMTOnly) {
    errs() << argv[0] << ": --dump-smt-only stops before the solver picks the bound; use --unwind\n";
    return 1;
  }
  if (unwind || noUnwindingAssertions) {
    errs() << argv[0] << ": --incremental-bmc chooses the bound and checks it\n";
    return 1;
  }
  for (unsigned k = 1; unlimitedKSteps || k <= maxKStep; k += kStep) {
    // A new walk and solver per bound, as ESBMC: the walk merges the paths
    // leaving a loop, so a larger bound changes everything after it. (One
    // solver with a scope per bound was measured and not kept; see
    // docs/survey/2026-11-22-incremental-bmc.md.)
    progress("Checking bound k = " + std::to_string(k));
    if (int r = bmc(*fn, k); r != BoundTooSmall)
      return r;
  }
  return unknown("no verdict up to bound " + std::to_string(maxKStep));
}
