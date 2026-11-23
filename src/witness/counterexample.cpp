#include "witness/counterexample.h"

#include <llvm/Support/WithColor.h>
#include <map>

using namespace llvm;

namespace vereda::witness {

static const char *separator = "----------------------------------------------------";

// The value in decimal and the grouped bits, e.g. {"99", "00000000 ...
// 01100011"}; bits are empty for Booleans and the value is "?" when the
// model has none.
static std::pair<std::string, std::string> valueOf(camada::SMTSolver &s,
                                                   const walker::Step &step) {
  if (step.value->isBoolSort()) {
    auto b = s.getBool(step.value);
    return {b ? (b.value() ? "true" : "false") : "?", ""};
  }
  auto bits = s.getBVInBin(step.value);
  if (!bits)
    return {"?", ""};
  std::string value = step.value->getWidth() > 64 ? "0b" + bits.value()
                      : step.isUnsigned ? std::to_string(s.getBVUnsigned(step.value).value())
                                        : std::to_string(s.getBV(step.value).value());
  std::string grouped;
  for (size_t i = 0; i < bits.value().size(); ++i) {
    if (i && (bits.value().size() - i) % 8 == 0)
      grouped += ' ';
    grouped += bits.value()[i];
  }
  return {value, grouped};
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
      os << "\nAssumption:\n  " << step.loc.str() << "\n  " << step.text << "\n";
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
    auto [value, bits] = valueOf(s, shown);
    std::string &prev = last[step.loc.function + "::" + shown.text];
    if (prev == value)
      continue;
    prev = value;
    os << "\nState " << ++n << " " << step.loc.str() << " thread 0\n" << separator << "\n  "
       << shown.text << " = " << value;
    if (!bits.empty())
      WithColor(os, raw_ostream::BRIGHT_BLACK) << " (" << bits << ")";
    os << "\n";
  }
  const checks::Check &c = checks[violated];
  os << "\nState " << ++n << " " << c.loc.str() << " thread 0\n" << separator << "\n"
     << "Violated property:\n  " << c.loc.str() << "\n  " << c.comment << "\n";
  if (!c.expr.empty())
    os << "  " << c.expr << "\n";
  os << "\n";
}

} // namespace vereda::witness
