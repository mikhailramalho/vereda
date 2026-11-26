// Stage checks: property checks collected during the walk. Each is a term
// that is satisfiable only if the property is violated, tagged with its
// property and source location so one walk can serve several properties.
#pragma once
#include "encode/terms.h"
#include "ir/debug.h"
#include <string>
#include <vector>

namespace vereda::checks {

enum class Property { Assertion, ArithmeticOverflow, DivisionByZero, CountZero, ArrayBounds, Unwinding, Recursion };

struct Check {
  Property property;
  ir::SourceLoc loc;
  std::string comment; // the callee, e.g. "__VERIFIER_assert"
  std::string expr;    // the asserted expression's source text, if any
  unsigned index;      // assertions per function from 1; loops by number
  encode::Term violated;
  // An unwinding assertion every path fails, of a loop whose trip count
  // SCEV knows: the smallest bound that unwinds it; 0 otherwise.
  unsigned unwindsAt = 0;

  bool isUnwinding() const {
    return property == Property::Unwinding || property == Property::Recursion;
  }

  // ESBMC's property id, e.g. "main.assertion.1" or "main.unwind.0".
  std::string id() const {
    const char *kind = property == Property::Assertion ? "assertion"
                       : property == Property::ArithmeticOverflow ? "arithmetic-overflow"
                       : property == Property::DivisionByZero ? "division-by-zero"
                       : property == Property::CountZero ? "count-zero"
                       : property == Property::ArrayBounds ? "array-bounds-violated"
                       : property == Property::Unwinding ? "unwind"
                                                          : "recursion";
    return loc.function + "." + kind + "." + std::to_string(index);
  }
};

using Checks = std::vector<Check>;

} // namespace vereda::checks
