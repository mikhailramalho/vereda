// Stage witness: the counterexample as an SV-COMP violation witness.
#pragma once
#include "checks/checks.h"
#include "walker/trace.h"
#include <camada/camada.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>

namespace vereda::witness {

// The path to the first violation of checks[violated] the model shows, as
// a violation witness, YAML format 2.0 (sv-witnesses), for unreach-call
// from entry: a function_return waypoint for each integer a
// __VERIFIER_nondet_ function of known sign returned, then the target. m
// gives the input files and the data model. An error says why there is no
// witness: the violation is not a call to reach_error, or it is outside
// the input files. The solver must hold the model, as for
// printCounterexample.
llvm::Expected<std::string> violationWitness(camada::SMTSolver &s, const walker::Trace &trace,
                                             const checks::Checks &checks, unsigned violated,
                                             const llvm::Module &m, llvm::StringRef entry);

} // namespace vereda::witness
