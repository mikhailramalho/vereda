// Stage witness: the counterexample as an SV-COMP violation witness.
#pragma once
#include "checks/checks.h"
#include "solver/model.h"
#include "walker/trace.h"
#include <cstdint>
#include <llvm/ADT/STLFunctionalExtras.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Error.h>
#include <llvm/Support/raw_ostream.h>
#include <string>
#include <utility>
#include <vector>

namespace llvm {
class Module;
}

namespace vereda::witness {

// What violationWitness needs from the LLVM module, extracted before the
// module is freed ahead of solving: compile-unit file paths, pointer
// bitwidth, and each nondet declaration's return type and sign attributes.
struct WitnessModuleInfo {
  enum class RetKind : uint8_t { Other, Pointer, Float, Double, LongDouble, Integer };
  struct NondetRet {
    RetKind kind = RetKind::Other;
    bool hasSExt = false;
    bool hasZExt = false;
  };
  std::vector<std::pair<std::string, std::string>> inputFiles; // (debug filename, absolute path)
  unsigned pointerBitWidth = 64;
  llvm::StringMap<NondetRet> nondetReturns;
};

WitnessModuleInfo extractModuleInfo(const llvm::Module &m);

// The path to the first violation of checks[violated] the model shows, as
// a violation witness, YAML format 2.0 (sv-witnesses), for unreach-call
// from entry: a function_return waypoint for each value a
// __VERIFIER_nondet_ function returned that a constant can state and a
// branching waypoint for each if, while and for condition but one right
// before the target, in path order, then the target. info gives the input
// files, the data model and the nondet functions' return types. An error
// says why there is no witness: the violation is not a call to reach_error,
// or it is outside the input files. Once validated, emit is called with a
// writer that streams the YAML directly to its output stream without
// copying the waypoint buffer. The model is the one from that check, as for
// printCounterexample.
llvm::Error violationWitness(
    solver::Model &model, const walker::Trace &trace, const checks::Checks &checks,
    unsigned violated, const WitnessModuleInfo &info, llvm::StringRef entry,
    llvm::function_ref<void(llvm::function_ref<void(llvm::raw_ostream &)>)> emit);

} // namespace vereda::witness
