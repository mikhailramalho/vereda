// Stage walker: the trace, one step per source-level event in walk order.
// The witness stage reads it back under the solver's model.
#pragma once
#include "ir/debug.h"
#include <camada/camada.h>
#include <string>
#include <vector>

namespace vereda::walker {

struct Step {
  enum Kind { Assignment, Assumption, Check } kind;
  camada::SMTExprRef guard; // path condition at the step; for Check, the violation
  ir::SourceLoc loc;
  std::string text;         // the variable, or the assumed expression
  camada::SMTExprRef value; // assignments only
  bool isUnsigned = false;  // from the debug type; bitvectors carry no sign
  unsigned check = 0;       // for Check, the index of the claim in the checks
  // A store into a named variable kept in memory: the variable's debug type
  // and the byte offset written, from which the witness spells "a[2].x".
  const llvm::DIType *type = nullptr;
  camada::SMTExprRef offset;
};

using Trace = std::vector<Step>;

} // namespace vereda::walker
