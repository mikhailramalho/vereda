// Stage walker: the trace, one step per source-level event in walk order.
// The witness stage reads it back under the solver's model.
#pragma once
#include "encode/terms.h"
#include "ir/debug.h"
#include <string>
#include <vector>

namespace vereda::walker {

struct Step {
  // Call: the value a bodyless __VERIFIER_nondet_ function returned, for
  // the witness. Branch: a branch that decides a source condition (see
  // ir::branchSite) taken one way, for the witness.
  enum Kind { Assignment, Assumption, Check, Call, Branch } kind;
  // Path condition at the step; for Check, the violation; for Branch, the
  // path condition with the branch taken.
  encode::Term guard;
  // For Check, where the witness puts the violation: the call to
  // reach_error when the check is inside it. For Branch, ir::BranchSite's.
  ir::SourceLoc loc;
  // The variable, the assumed expression or the callee; for Check,
  // "reach_error" when the violation is a call to it; for Branch, the
  // condition's value, "true" or "false".
  std::string text;
  encode::Term value; // assignments and calls only
  bool isUnsigned = false;  // from the debug type; bitvectors carry no sign
  unsigned check = 0;       // for Check, the index of the claim in the checks
  // A store into a named variable kept in memory: the variable's debug type
  // and the byte offset written, from which the witness spells "a[2].x".
  const llvm::DIType *type = nullptr;
  encode::Term offset;
};

using Trace = std::vector<Step>;

} // namespace vereda::walker
