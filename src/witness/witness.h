// Stage witness: the counterexample as an SV-COMP violation witness.
#pragma once
#include "checks/checks.h"
#include "walker/trace.h"
#include "solver/model.h"
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>

namespace vereda::witness {

// The path to the first violation of checks[violated] the model shows, as
// a violation witness, YAML format 2.0 (sv-witnesses), for unreach-call
// from entry: a function_return waypoint for each value a
// __VERIFIER_nondet_ function returned that a constant can state and a
// branching waypoint for each if, while and for condition but one right
// before the target, in path order, then the target. m gives the input files, the data model and the nondet
// functions' return types. An error says why there is no witness: the
// violation is not a call to reach_error, or it is outside the input
// files. The model is the one from that check, as for printCounterexample.
llvm::Expected<std::string> violationWitness(solver::Model &model, const walker::Trace &trace,
                                             const checks::Checks &checks, unsigned violated,
                                             const llvm::Module &m, llvm::StringRef entry);

} // namespace vereda::witness
