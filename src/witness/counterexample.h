// Stage witness: the counterexample, read from the solver's model.
#pragma once
#include "checks/checks.h"
#include "walker/trace.h"
#include "solver/model.h"
#include <llvm/Support/raw_ostream.h>

namespace vereda::witness {

// Prints the steps the model reaches with their values, up to the first
// instance of checks[violated] the model violates. State assignments include
// grouped binary values as inline comments; assumptions and the violated property have their
// own plain-text blocks. The model must be the one from the check that found
// the violation.
void printCounterexample(solver::Model &model, const walker::Trace &trace,
                         const checks::Checks &checks, unsigned violated,
                         llvm::raw_ostream &os, bool showLibraryStates);

} // namespace vereda::witness
