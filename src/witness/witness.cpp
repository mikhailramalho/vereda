#include "witness/witness.h"

#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/StringSet.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SHA256.h>
#include <algorithm>
#include <ctime>
#include <map>
#include <random>

using namespace llvm;

namespace vereda::witness {

static std::string quote(StringRef s) {
  std::string out = "\"";
  for (char c : s) {
    if (c == '"' || c == '\\')
      out += '\\';
    out += c;
  }
  return out + "\"";
}

// A version 4 UUID (RFC 4122).
static std::string uuid() {
  std::random_device rd;
  uint8_t b[16];
  for (uint8_t &x : b)
    x = rd();
  b[6] = (b[6] & 0x0f) | 0x40;
  b[8] = (b[8] & 0x3f) | 0x80;
  std::string hex = toHex(b, /*LowerCase=*/true);
  return hex.substr(0, 8) + "-" + hex.substr(8, 4) + "-" + hex.substr(12, 4) + "-" +
         hex.substr(16, 4) + "-" + hex.substr(20);
}

// An input file's text and where each of its lines starts: a long trace
// has a waypoint per iteration, and scanning the file for each one's line
// took seconds on a file of thousands of lines.
namespace {
struct Source {
  std::unique_ptr<MemoryBuffer> buf;
  std::vector<size_t> starts; // per line, the offset of its first character

  explicit Source(std::unique_ptr<MemoryBuffer> b) : buf(std::move(b)), starts{0} {
    StringRef text = buf->getBuffer();
    for (size_t i = 0; i < text.size(); ++i)
      if (text[i] == '\n')
        starts.push_back(i + 1);
  }
  StringRef text() const { return buf->getBuffer(); }
  // Line n, counted from 1, without its '\n'.
  StringRef line(unsigned n) const {
    return text().slice(starts[n - 1], n < starts.size() ? starts[n] - 1 : text().size());
  }
};
} // namespace

// The task's source files with their contents, by the name debug
// locations use; a file that cannot be read is left out.
static std::map<std::string, Source> inputFiles(const Module &m) {
  std::map<std::string, Source> files;
  for (const DICompileUnit *cu : m.debug_compile_units()) {
    SmallString<128> path(cu->getFilename());
    sys::fs::make_absolute(cu->getDirectory(), path);
    if (auto buf = MemoryBuffer::getFile(path))
      files.insert_or_assign(cu->getFilename().str(), Source(std::move(*buf)));
  }
  return files;
}

// Whether text holds a call: a name followed by '(' that is not a keyword.
static bool hasCall(StringRef text) {
  static const StringSet<> keywords = {"if", "while", "for", "switch", "return", "sizeof"};
  for (size_t i = 0; i < text.size(); ++i) {
    if (!isAlpha(text[i]) && text[i] != '_')
      continue;
    size_t end = i;
    while (end < text.size() && (isAlnum(text[end]) || text[end] == '_'))
      ++end;
    if (text.drop_front(end).ltrim().starts_with("(") && !keywords.contains(text.slice(i, end)))
      return true;
    i = end;
  }
  return false;
}

// The column of a function_return waypoint for the call to callee at loc,
// or 0 to leave it out. The format puts it at the ')' that closes the call,
// but UAutomizer 0.3.2 has no call there: it matches the waypoint at the end
// of the statements around the call, which is that ')' for an assignment
// or a condition but the ';' of a declaration. Without a column both
// validators take the line, CPAchecker 4.3 at its first call, so the column
// is left out unless another call comes before this one on the line. It is
// left out too when the source there is not that call on one line, as for
// a call a macro expands to, whose location is the macro's name.
static unsigned returnColumn(const Source &file, const ir::SourceLoc &loc, StringRef callee) {
  if (loc.line > file.starts.size() || !loc.col)
    return 0;
  StringRef line = file.line(loc.line);
  if (!hasCall(line.take_front(loc.col - 1)))
    return 0;
  StringRef rest = line.drop_front(loc.col - 1);
  if (!rest.consume_front(callee))
    return 0;
  rest = rest.ltrim();
  int depth = 0;
  for (size_t i = line.size() - rest.size(); i < line.size(); ++i) {
    if (line[i] == '(')
      ++depth;
    else if (!depth) // the name was a prefix of another identifier
      return 0;
    else if (line[i] == ')' && --depth == 0)
      return i + 1;
  }
  return 0;
}

// Where format 2.0 puts the branching waypoint of a branch step: at the
// keyword of its statement. A loop's step is there already; an if's is at
// the start of its condition, after "if (". None when the text is not that,
// as for a statement a macro expands to.
static std::optional<ir::SourceLoc> keyword(const Source &file, ir::SourceLoc loc) {
  StringRef text = file.text();
  if (loc.line > file.starts.size() || !loc.col)
    return std::nullopt;
  size_t pos = file.starts[loc.line - 1] + loc.col - 1;
  if (pos >= text.size())
    return std::nullopt;
  auto isIdent = [](char c) { return isAlnum(c) || c == '_'; };
  StringRef word = text.drop_front(pos).take_while(isIdent);
  if (word == "while" || word == "for")
    return loc;
  StringRef before = text.take_front(pos).rtrim();
  if (!before.consume_back("("))
    return std::nullopt;
  before = before.rtrim();
  if (!before.ends_with("if") || (before.size() > 2 && isIdent(before[before.size() - 3])))
    return std::nullopt;
  size_t start = before.size() - 2;
  auto next = std::upper_bound(file.starts.begin(), file.starts.end(), start);
  loc.line = next - file.starts.begin();
  loc.col = start - next[-1] + 1;
  return loc;
}

// The SV-COMP nondet functions whose sign C fixes; pthread_t, sector_t and
// the like depend on the program's typedefs.
static const StringSet<> signedTypes = {"char", "short", "int", "long", "longlong", "loff_t",
                                        "int128"};
static const StringSet<> unsignedTypes = {"uchar", "ushort", "uint", "unsigned", "ulong",
                                          "ulonglong", "u8", "u16", "u32", "u64",
                                          "size_t", "uint128"};

// \result == v, v read in the given sign, or empty when v needs more than
// 64 bits: C has no wider constant, and a constant expression that builds
// one, such as ((unsigned __int128)hi << 64 | lo), is not ACSL.
static std::string equals(const APInt &v, bool isSigned) {
  if (isSigned ? !v.isSignedIntN(64) : !v.isIntN(64))
    return "";
  if (!isSigned) {
    uint64_t u = v.getZExtValue();
    // A decimal constant above INT64_MAX needs the suffix to have a type.
    return "\\result == " + std::to_string(u) + (u > INT64_MAX ? "U" : "");
  }
  int64_t i = v.getSExtValue();
  // The lowest value has no decimal constant, its magnitude does not fit;
  // a missing waypoint would shift the later ones at its call site (#76).
  return "\\result == " + (i == INT64_MIN ? "-9223372036854775807 - 1" : std::to_string(i));
}

// A hexadecimal floating constant, which is exact where a decimal one is
// rounded.
static std::string hex(const APFloat &x) {
  char buf[64];
  x.convertToHexString(buf, 0, /*UpperCase=*/false, APFloat::rmNearestTiesToEven);
  return buf;
}

// The constraint a function_return waypoint puts on the model's value of a
// call step, of the form \result <op> <constant> that format 2.0 allows, or
// empty when none holds of that value alone.
static std::string constraint(solver::Model &model, const walker::Step &step, const Module &m) {
  const APInt *value = model.value(step.value);
  if (!value)
    return "";
  if (step.value->isBoolSort()) // a _Bool
    return std::string("\\result == ") + (value->isOne() ? "1" : "0");
  const APInt &v = *value;
  const Function *f = m.getFunction(step.text);
  Type *ret = f->getReturnType();
  // A pointer is an object and an offset, and no constant names an object;
  // \result != 0 still keeps the waypoint at its call (#76).
  if (ret->isPointerTy())
    return v.isZero() ? "\\result == 0" : "\\result != 0";
  if (ret->isFloatTy() || ret->isDoubleTy()) {
    // No constant compares equal to a NaN, and an infinity is beyond the
    // largest finite value. The sign of a zero does not show: -0.0 == 0.0.
    APFloat x(ret->getFltSemantics(), v);
    if (x.isNaN())
      return "";
    if (x.isInfinity())
      return std::string("\\result ") + (x.isNegative() ? "< " : "> ") +
             hex(APFloat::getLargest(x.getSemantics(), x.isNegative()));
    return "\\result == " + hex(x);
  }
  // The ABI returns an __int128 as { i64, i64 }, whose packed bits, the
  // first field lowest, are the integer's.
  StringRef type = step.text;
  type.consume_front("__VERIFIER_nondet_");
  if (!ret->isIntegerTy() && !signedTypes.contains(type) && !unsignedTypes.contains(type))
    return "";
  // C promotes a type narrower than int before the comparison, so 200 and
  // -56 differ for a char. The name gives the sign, or else the extension
  // the ABI asks of a narrow result. A value whose top bit is clear reads
  // the same in either sign; one whose top bit is set, of a type whose
  // sign neither tells (pthread_t), gets no constraint.
  if (signedTypes.contains(type) || f->hasRetAttribute(Attribute::SExt))
    return equals(v, true);
  if (unsignedTypes.contains(type) || f->hasRetAttribute(Attribute::ZExt) || v.isNonNegative())
    return equals(v, false);
  return "";
}

static void waypoint(raw_ostream &os, StringRef type, const ir::SourceLoc &loc, unsigned col,
                     StringRef constraint) {
  os << "  - segment:\n"
     << "    - waypoint:\n"
     << "        type: " << quote(type) << "\n"
     << "        action: \"follow\"\n";
  if (!constraint.empty()) {
    os << "        constraint:\n"
       << "          value: " << quote(constraint) << "\n";
    if (type != "branching") // whose constraint is a value, not an expression
      os << "          format: \"acsl_expression\"\n";
  }
  os << "        location:\n"
     << "          file_name: " << quote(loc.file) << "\n"
     << "          line: " << loc.line << "\n";
  if (col)
    os << "          column: " << col << "\n";
  os << "          function: " << quote(loc.function) << "\n";
}

Expected<std::string> violationWitness(solver::Model &model, const walker::Trace &trace,
                                       const checks::Checks &checks, unsigned violated,
                                       const Module &m, StringRef entry) {
  auto files = inputFiles(m);
  // A waypoint must point into an input file: a call in a header or in a
  // library model gets none, and so does a value with no C constant.
  std::string content;
  raw_string_ostream os(content);
  const walker::Step *target = nullptr;
  size_t lastBranching = std::string::npos; // where the last waypoint starts, if branching
  for (const walker::Step &step : trace) {
    bool isCheck = step.kind == walker::Step::Check;
    if ((isCheck && step.check != violated) || step.kind == walker::Step::Assignment ||
        step.kind == walker::Step::Assumption)
      continue;
    if (!model.reaches(step.guard))
      continue;
    if (isCheck) {
      target = &step;
      break;
    }
    auto file = files.find(step.loc.file);
    if (file == files.end() || !step.loc.line)
      continue;
    if (step.kind == walker::Step::Branch) {
      if (std::optional<ir::SourceLoc> loc = keyword(file->second, step.loc)) {
        lastBranching = content.size();
        waypoint(os, "branching", *loc, loc->col, step.text);
      }
      continue;
    }
    std::string value = constraint(model, step, m);
    if (!value.empty()) {
      lastBranching = std::string::npos;
      waypoint(os, "function_return", step.loc,
               returnColumn(file->second, step.loc, step.text), value);
    }
  }
  // The witness states unreach-call, which only a call to reach_error
  // violates.
  if (!target || target->text != "reach_error")
    return createStringError("the violation (" + checks[violated].comment +
                             ") is not a call to reach_error");
  if (!files.count(target->loc.file))
    return createStringError("the call to reach_error is in " + target->loc.file +
                             ", not in an input file");
  // A branching waypoint right before the target says little the target
  // does not: mostly it is the if around the call to reach_error. With it,
  // CPAchecker 4.3's refinement fails on some tasks it confirms without it
  // (MathSAT cannot interpolate).
  if (lastBranching != std::string::npos)
    content.resize(lastBranching);
  waypoint(os, "target", target->loc, target->loc.col, "");

  char now[32];
  std::time_t t = std::time(nullptr);
  std::strftime(now, sizeof now, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&t));
  std::string out;
  raw_string_ostream w(out);
  w << "- entry_type: \"violation_sequence\"\n"
    << "  metadata:\n"
    << "    format_version: \"2.0\"\n"
    << "    uuid: " << quote(uuid()) << "\n"
    << "    creation_time: " << quote(now) << "\n"
    << "    producer:\n"
    << "      name: \"Vereda\"\n"
    << "      version: " << quote(VEREDA_VERSION) << "\n"
    << "    task:\n"
    << "      input_files:\n";
  for (auto &[name, source] : files)
    w << "      - " << quote(name) << "\n";
  w << "      input_file_hashes:\n";
  for (auto &[name, source] : files)
    w << "        " << quote(name) << ": "
      << quote(toHex(SHA256::hash(arrayRefFromStringRef(source.text())), /*LowerCase=*/true))
      << "\n";
  w << "      specification: "
    << quote("CHECK( init(" + entry.str() + "()), LTL(G ! call(reach_error())) )") << "\n"
    << "      data_model: "
    << (m.getDataLayout().getPointerSizeInBits() == 32 ? "\"ILP32\"" : "\"LP64\"") << "\n"
    << "      language: \"C\"\n"
    << "  content:\n"
    << content;
  return out;
}

} // namespace vereda::witness
