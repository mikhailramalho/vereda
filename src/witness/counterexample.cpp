#include "witness/counterexample.h"

#include <cstdint>
#include <llvm/Support/WithColor.h>
#include <map>

using namespace llvm;

namespace vereda::witness {

// The step's assignment as a C statement and the value's grouped bits, e.g.
// {"x = 99;", "00000000 ... 01100011"}; bits are empty for Booleans. A value
// C has no constant for (none in the model, or wider than 64 bits) makes the
// statement a comment.
static std::pair<std::string, std::string> assignment(camada::SMTSolver &s,
                                                      const walker::Step &step) {
  std::string lhs = step.text + " = ";
  if (step.value->isBoolSort()) {
    auto b = s.getBool(step.value);
    if (!b)
      return {"// " + lhs + "?", ""};
    return {lhs + (b.value() ? "true;" : "false;"), ""};
  }
  auto bits = s.getBVInBin(step.value);
  if (!bits)
    return {"// " + lhs + "?", ""};
  if (step.value->getWidth() > 64)
    return {"// " + lhs + "0b" + bits.value(), ""};
  std::string value;
  if (step.isUnsigned) {
    uint64_t u = s.getBVUnsigned(step.value).value();
    // An unsuffixed decimal constant past LLONG_MAX has no type.
    value = std::to_string(u) + (u > INT64_MAX ? "u" : "");
  } else {
    int64_t v = s.getBV(step.value).value();
    // -9223372036854775808 negates a constant that has no type.
    value = v == INT64_MIN ? "-9223372036854775807 - 1" : std::to_string(v);
  }
  std::string grouped;
  for (size_t i = 0; i < bits.value().size(); ++i) {
    if (i && (bits.value().size() - i) % 8 == 0)
      grouped += ' ';
    grouped += bits.value()[i];
  }
  return {lhs + value + ";", grouped};
}

void printCounterexample(camada::SMTSolver &s, const walker::Trace &trace,
                         const checks::Checks &checks, unsigned violated, raw_ostream &os) {
  os << "\n";
  WithColor(os, raw_ostream::RED, /*Bold=*/true) << "[Counterexample]";
  os << "\n\n";
  unsigned n = 0;
  // Merge points (loop heads, joins) repeat a variable's value under their
  // own debug record; print a variable only when its value changes.
  // This also hides a source assignment of the same value.
  std::map<std::string, std::string> last;
  for (const walker::Step &step : trace) {
    if (step.kind == walker::Step::Call ||
        (step.kind == walker::Step::Check && step.check != violated))
      continue;
    auto reached = s.getBool(step.guard);
    if (!reached || !reached.value())
      continue;
    if (step.kind == walker::Step::Check)
      break;
    if (step.kind == walker::Step::Assumption) {
      os << "\n/*\nAssumption:\n  " << step.loc.str() << "\n  " << step.text << "\n*/\n";
      continue;
    }
    walker::Step shown = step;
    if (step.type) { // a store into memory: name the element or member written
      auto off = s.getBVUnsigned(step.offset);
      std::optional<std::string> path;
      if (off)
        path = ir::memberPath(step.type, off.value(), step.value->getWidth() / 8, shown.isUnsigned);
      if (!path) // no single scalar to name (a bitfield's unit, a partial write)
        continue;
      shown.text += *path;
    }
    auto [stmt, bits] = assignment(s, shown);
    std::string &prev = last[step.loc.function + "::" + shown.text];
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
