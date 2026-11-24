// Stage checks: property checks collected during the walk. Each is a term
// that is satisfiable only if the property is violated, tagged with its
// property and source location so one walk can serve several properties.
#pragma once
#include "ir/debug.h"
#include <camada/camada.h>
#include <string>
#include <vector>

namespace vereda::checks {

enum class Property { Assertion, Unwinding, Recursion };

struct Check {
  Property property;
  ir::SourceLoc loc;
  std::string comment; // the callee, e.g. "__VERIFIER_assert"
  std::string expr;    // the asserted expression's source text, if any
  unsigned index;      // assertions per function from 1; loops by number
  camada::SMTExprRef violated;
  // An unwinding assertion every path fails, of a loop whose trip count
  // SCEV knows: the smallest bound that unwinds it; 0 otherwise.
  unsigned unwindsAt = 0;

  // ESBMC's property id, e.g. "main.assertion.1" or "main.unwind.0".
  std::string id() const {
    const char *kind = property == Property::Assertion ? "assertion"
                       : property == Property::Unwinding ? "unwind"
                                                          : "recursion";
    return loc.function + "." + kind + "." + std::to_string(index);
  }
};

using Checks = std::vector<Check>;

} // namespace vereda::checks
