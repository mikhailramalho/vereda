// Stage walker: the trace, one step per source-level event in walk order.
// The witness stage reads it back under the solver's model.
#pragma once
#include "ir/debug.h"
#include <camada/camada.h>
#include <string>
#include <vector>

namespace vereda::walker {

struct Step {
  // Call: an integer a bodyless __VERIFIER_nondet_ function returned, for
  // the witness.
  enum Kind { Assignment, Assumption, Check, Call } kind;
  camada::SMTExprRef guard; // path condition at the step; for Check, the violation
  // For Check, where the witness puts the violation: the call to
  // reach_error when the check is inside it.
  ir::SourceLoc loc;
  // The variable, the assumed expression or the callee; for Check,
  // "reach_error" when the violation is a call to it.
  std::string text;
  camada::SMTExprRef value; // assignments and calls only
  bool isUnsigned = false;  // from the debug type; bitvectors carry no sign
  unsigned check = 0;       // for Check, the index of the claim in the checks
  // A store into a named variable kept in memory: the variable's debug type
  // and the byte offset written, from which the witness spells "a[2].x".
  const llvm::DIType *type = nullptr;
  camada::SMTExprRef offset;
};

using Trace = std::vector<Step>;

} // namespace vereda::walker
