#include "witness/counterexample.h"

#include <cstdint>
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/WithColor.h>

using namespace llvm;

namespace vereda::witness {

// The assignment of value to text as a C statement and the value's grouped
// bits, e.g. {"x = 99;", "00000000 ... 01100011"}; bits are empty for
// Booleans. A value C has no constant for (none in the model, or wider than
// 64 bits) makes the statement a comment.
static std::pair<std::string, std::string> assignment(solver::Model &model,
                                                      const camada::SMTExprRef &value,
                                                      const std::string &text, bool isUnsigned) {
  std::string lhs = text + " = ";
  const APInt *v = model.value(value);
  if (!v)
    return {"// " + lhs + "?", ""};
  if (value->isBoolSort())
    return {lhs + (v->isOne() ? "true;" : "false;"), ""};
  std::string bits; // by hand: APInt::toString in base 2 cost more than the rest
  for (unsigned i = v->getBitWidth(); i-- > 0;)
    bits += (*v)[i] ? '1' : '0';
  if (v->getBitWidth() > 64)
    return {"// " + lhs + "0b" + bits, ""};
  std::string decimal;
  if (isUnsigned) {
    uint64_t u = v->getZExtValue();
    // An unsuffixed decimal constant past LLONG_MAX has no type.
    decimal = std::to_string(u) + (u > INT64_MAX ? "u" : "");
  } else {
    int64_t i = v->getSExtValue();
    // -9223372036854775808 negates a constant that has no type.
    decimal = i == INT64_MIN ? "-9223372036854775807 - 1" : std::to_string(i);
  }
  std::string grouped;
  for (size_t i = 0; i < bits.size(); ++i) {
    if (i && (bits.size() - i) % 8 == 0)
      grouped += ' ';
    grouped += bits[i];
  }
  return {lhs + decimal + ";", grouped};
}

void printCounterexample(solver::Model &model, const walker::Trace &trace,
                         const checks::Checks &checks, unsigned violated, raw_ostream &os) {
  os << "\n";
  WithColor(os, raw_ostream::RED, /*Bold=*/true) << "[Counterexample]";
  os << "\n\n";
  unsigned n = 0;
  // Merge points (loop heads, joins) repeat a variable's value under their
  // own debug record; print a variable only when its value changes.
  // This also hides a source assignment of the same value.
  StringMap<std::string> last;
  for (const walker::Step &step : trace) {
    if (step.kind == walker::Step::Call || step.kind == walker::Step::Branch ||
        (step.kind == walker::Step::Check && step.check != violated))
      continue;
    if (!model.reaches(step.guard))
      continue;
    if (step.kind == walker::Step::Check)
      break;
    if (step.kind == walker::Step::Assumption) {
      os << "\n/*\nAssumption:\n  " << step.loc.str() << "\n  " << step.text << "\n*/\n";
      continue;
    }
    std::string text = step.text;
    bool isUnsigned = step.isUnsigned;
    if (step.type) { // a store into memory: name the element or member written
      const APInt *off = model.value(step.offset);
      std::optional<std::string> path;
      if (off)
        path = ir::memberPath(step.type, off->getZExtValue(), step.value->getWidth() / 8,
                              isUnsigned);
      if (!path) // no single scalar to name (a bitfield's unit, a partial write)
        continue;
      text += *path;
    }
    auto [stmt, bits] = assignment(model, step.value, text, isUnsigned);
    std::string &prev = last[step.loc.function + "::" + text];
    if (prev == stmt)
      continue;
    prev = stmt;
    os << "\n// State " << ++n << " " << step.loc.str() << " thread 0\n  " << stmt;
    if (!bits.empty())
      WithColor(os, raw_ostream::BRIGHT_BLACK) << " // " << bits;
    os << "\n";
  }
  const checks::Check &c = checks[violated];
  os << "\n// State " << ++n << " " << c.loc.str() << " thread 0\n"
     << "/*\nViolated property:\n  " << c.loc.str() << "\n  " << c.comment << "\n";
  if (!c.expr.empty())
    os << "  " << c.expr << "\n";
  os << "*/\n\n";
}

} // namespace vereda::witness
