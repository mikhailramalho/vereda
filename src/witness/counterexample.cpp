#include "witness/counterexample.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/Support/WithColor.h>

// glibc before 2.41 declares its *f128 functions for GCC only (clang counts
// as GCC 4.2); the function is there all the same.
#if !__HAVE_FLOAT128
extern "C" int strfromf128(char *, size_t, const char *, __float128);
#endif

using namespace llvm;

namespace vereda::witness {

namespace {
// Last printed value of a variable, keyed by what makes the printed statement
// identical: checking this before formatting avoids string allocations on
// unchanged values at merge points and loop headers.
struct LastValue {
  enum Kind : uint8_t { Unset, Unknown, Bool, Int64, UInt64, Wide, Float } kind = Unset;
  const void *termId = nullptr;
  bool isUnsigned = false;
  uint64_t val = 0;
  std::string wide; // the bits, only when kind is Wide (binary) or Float (hex)
};
} // namespace

static void writeGroupedBits(raw_ostream &os, const APInt &v) {
  unsigned w = v.getBitWidth();
  char buf[144]; // at most 128 bits + 15 spaces
  size_t len = 0;
  for (unsigned i = w; i-- > 0;) {
    if (len && (i + 1) % 8 == 0)
      buf[len++] = ' ';
    buf[len++] = v[i] ? '1' : '0';
  }
  WithColor(os, raw_ostream::BRIGHT_BLACK) << " // " << StringRef(buf, len);
}

// A half, float, double, x86 long double or fp128 as ESBMC prints a float,
// as a C constant: %.6e with the exponent unpadded, a suffix (f for a float,
// L for a long double, Q for an fp128 as GCC and clang read __float128, none
// for a half), +INFINITY, -NAN and so on. Every format converts exactly to
// binary128, which glibc's strfromf128 rounds exactly (APFloat's toString
// truncates before it rounds to the digits asked for and gets the last digit
// wrong now and then).
static void writeFloat(raw_ostream &os, const APInt &v) {
  unsigned w = v.getBitWidth();
  APFloat f(w == 16   ? APFloat::IEEEhalf()
            : w == 32 ? APFloat::IEEEsingle()
            : w == 64 ? APFloat::IEEEdouble()
            : w == 80 ? APFloat::x87DoubleExtended()
                      : APFloat::IEEEquad(),
            v);
  const char *sign = f.isNegative() ? "-" : "+";
  if (f.isInfinity() || f.isNaN()) {
    os << sign << (f.isNaN() ? "NAN" : "INFINITY");
    return;
  }
  bool lost;
  f.convert(APFloat::IEEEquad(), APFloat::rmNearestTiesToEven, &lost);
  APInt bits = f.bitcastToAPInt();
  __float128 q;
  std::memcpy(&q, bits.getRawData(), sizeof q);
  char buf[48];
  strfromf128(buf, sizeof buf, "%.6e", q);
  auto [digits, exponent] = StringRef(buf).split('e');
  int e = std::atoi(exponent.data());
  os << digits << 'e' << (e < 0 ? '-' : '+') << std::abs(e)
     << (w == 32 ? "f" : w == 80 ? "L" : w == 128 ? "Q" : "");
}

void printCounterexample(solver::Model &model, const walker::Trace &trace,
                         const checks::Checks &checks, unsigned violated, raw_ostream &os,
                         bool showLibraryStates) {
  os << "\n";
  WithColor(os, raw_ostream::RED, /*Bold=*/true) << "[COUNTEREXAMPLE]";
  os << "\n";
  unsigned n = 0;
  StringMap<std::string> demangled;
  auto demangleFn = [&](StringRef fn) -> StringRef {
    auto [it, fresh] = demangled.try_emplace(fn);
    if (fresh)
      it->second = llvm::demangle(fn);
    return it->second;
  };
  auto writeLoc = [&](const ir::SourceLoc &loc) {
    StringRef fn = demangleFn(loc.function);
    if (loc.file.empty() || loc.file == "<unknown>") {
      os << "function " << fn;
      return;
    }
    os << "file " << loc.file << " line " << loc.line;
    if (loc.col)
      os << " column " << loc.col;
    os << " function " << fn;
  };

  // Merge points (loop heads, joins) repeat a variable's value under their
  // own debug record; print a variable only when its value changes.
  // This also hides a source assignment of the same value.
  StringMap<LastValue> last;
  SmallString<128> keyBuf;
  std::string textBuf;
  for (const walker::Step &step : trace) {
    if (step.kind == walker::Step::Call || step.kind == walker::Step::Branch ||
        (step.kind == walker::Step::Check && step.check != violated))
      continue;
    if (!model.reaches(step.guard))
      continue;
    if (step.kind == walker::Step::Check)
      break;
    const ir::SourceLoc &loc = trace.loc(step);
    if (step.kind == walker::Step::Assumption) {
      os << "\nAssumption:\n  ";
      writeLoc(loc);
      os << "\n  " << trace.text(step) << "\n";
      continue;
    }
    StringRef file = loc.file;
    if (!showLibraryStates && file.consume_front("/rustc/")) {
      auto [hash, rest] = file.split('/');
      if (!hash.empty() && rest.starts_with("library/"))
        continue;
    }
    StringRef text = trace.text(step);
    bool isUnsigned = step.isUnsigned;
    if (step.type) { // a store into memory: name the element or member written
      const APInt *off = model.value(step.offset);
      std::optional<std::string> path;
      if (off)
        path = ir::memberPath(step.type, off->getZExtValue(), step.value.width() / 8, isUnsigned);
      if (!path) // no single scalar to name (a bitfield's unit, a partial write)
        continue;
      textBuf.assign(text.data(), text.size());
      textBuf += *path;
      text = textBuf;
    }
    keyBuf.clear();
    keyBuf.append(loc.function);
    keyBuf.append("::");
    keyBuf.append(text);
    LastValue &prev = last[keyBuf];
    if (prev.kind != LastValue::Unset && prev.termId == step.value.id() &&
        prev.isUnsigned == isUnsigned)
      continue;

    const APInt *v = model.value(step.value);
    LastValue::Kind kind;
    uint64_t val = 0;
    std::string wideBits;
    if (!v) {
      kind = LastValue::Unknown;
    } else if (step.value.isBool()) {
      kind = LastValue::Bool;
      val = v->isOne();
    } else if (step.isFloat && (v->getBitWidth() == 16 || v->getBitWidth() == 32 ||
                                v->getBitWidth() == 64 || v->getBitWidth() == 80 ||
                                v->getBitWidth() == 128)) {
      kind = LastValue::Float;
      wideBits = toString(*v, 16, false);
    } else if (v->getBitWidth() > 64) {
      kind = LastValue::Wide;
      wideBits.reserve(v->getBitWidth());
      for (unsigned i = v->getBitWidth(); i-- > 0;)
        wideBits += (*v)[i] ? '1' : '0';
    } else if (isUnsigned) {
      uint64_t u = v->getZExtValue();
      kind = u > INT64_MAX ? LastValue::UInt64 : LastValue::Int64;
      val = u;
    } else {
      kind = LastValue::Int64;
      val = static_cast<uint64_t>(v->getSExtValue());
    }

    bool same = prev.kind == kind &&
                (kind == LastValue::Wide || kind == LastValue::Float ? prev.wide == wideBits
                                                                     : prev.val == val);
    if (same) {
      prev.termId = step.value.id();
      prev.isUnsigned = isUnsigned;
      continue;
    }
    prev.kind = kind;
    prev.termId = step.value.id();
    prev.isUnsigned = isUnsigned;
    prev.val = val;
    if (kind == LastValue::Wide || kind == LastValue::Float)
      prev.wide = std::move(wideBits);
    else
      prev.wide.clear();

    os << "\nState " << ++n << " ";
    writeLoc(loc);
    os << " thread 0\n  ";
    switch (kind) {
    case LastValue::Unknown:
      os << text << " = ?";
      break;
    case LastValue::Bool:
      os << text << " = " << (val ? "true;" : "false;");
      break;
    case LastValue::Float:
      os << text << " = ";
      writeFloat(os, *v);
      os << ";";
      writeGroupedBits(os, *v);
      break;
    case LastValue::Wide:
      os << text << " = 0b" << prev.wide;
      break;
    case LastValue::UInt64:
      // An unsuffixed decimal constant past LLONG_MAX has no type.
      os << text << " = " << val << "u;";
      writeGroupedBits(os, *v);
      break;
    case LastValue::Int64: {
      int64_t i = static_cast<int64_t>(val);
      // -9223372036854775808 negates a constant that has no type.
      if (i == INT64_MIN)
        os << text << " = -9223372036854775807 - 1;";
      else
        os << text << " = " << i << ";";
      writeGroupedBits(os, *v);
      break;
    }
    case LastValue::Unset:
      break;
    }
    os << "\n";
  }
  const checks::Check &c = checks[violated];
  os << "\nViolated property:\n  ";
  writeLoc(c.loc);
  os << "\n  " << c.comment << "\n";
  if (!c.expr.empty())
    os << "  " << c.expr << "\n";
}

} // namespace vereda::witness
