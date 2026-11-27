// Stage checks: property checks collected during the walk. Each is a term
// that is satisfiable only if the property is violated, tagged with its
// property and source location so one walk can serve several properties.
#pragma once
#include "encode/terms.h"
#include "ir/debug.h"
#include <algorithm>
#include <map>
#include <string>
#include <tuple>
#include <vector>

namespace vereda::checks {

enum class Property { Assertion, ArithmeticOverflow, Shift, DivisionByZero, CountZero, FloatCastOverflow, Alignment, UninitialisedRead, ArrayBounds, Unwinding, Recursion };

struct Check {
  Property property;
  ir::SourceLoc loc;
  std::string comment; // the callee, e.g. "__VERIFIER_assert"
  std::string expr;    // the asserted expression's source text, if any
  unsigned index;      // per function and kind from 1 (number); loops by number
  encode::Term violated;
  // An unwinding assertion of a loop whose maximum trip count SCEV knows:
  // the smallest bound that unwinds it; 0 otherwise.
  unsigned unwindsAt = 0;

  bool isUnwinding() const {
    return property == Property::Unwinding || property == Property::Recursion;
  }

  // ESBMC's property class, which ESBMC derives from the comment: a comment
  // its table does not name, as of the count-zero, float-cast and alignment
  // checks, makes an assertion.
  const char *kind() const {
    switch (property) {
    case Property::ArithmeticOverflow: return "arithmetic-overflow";
    case Property::Shift: return "shift-undefined-behavior";
    case Property::DivisionByZero: return "division-by-zero";
    case Property::UninitialisedRead: return "uninitialised-variable";
    case Property::ArrayBounds: return "array-bounds-violated";
    case Property::Unwinding: return "unwind";
    case Property::Recursion: return "recursion";
    default: return "assertion";
    }
  }

  // ESBMC's property id, e.g. "main.assertion.1" or "main.unwind.0".
  std::string id() const { return loc.function + "." + kind() + "." + std::to_string(index); }
};

// The order of ESBMC's results table: by file, function, position, comment.
inline bool before(const Check &a, const Check &b) {
  return std::tie(a.loc.file, a.loc.function, a.loc.line, a.loc.col, a.comment) <
         std::tie(b.loc.file, b.loc.function, b.loc.line, b.loc.col, b.comment);
}

using Checks = std::vector<Check>;

// Numbers each function's checks of one kind from 1 in that order, as ESBMC
// does, so an id does not depend on the order of the walk. Unwinding
// assertions keep their loop's number.
inline void number(Checks &checks) {
  std::vector<Check *> order;
  for (Check &c : checks)
    if (!c.isUnwinding())
      order.push_back(&c);
  std::stable_sort(order.begin(), order.end(),
                   [](const Check *a, const Check *b) { return before(*a, *b); });
  std::map<std::pair<std::string, std::string>, unsigned> counts;
  for (Check *c : order)
    c->index = ++counts[{c->loc.function, c->kind()}];
}

} // namespace vereda::checks
