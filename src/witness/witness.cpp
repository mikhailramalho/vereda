#include "witness/witness.h"

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/Hashing.h>
#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/StringSet.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/SHA256.h>
#include <algorithm>
#include <ctime>
#include <map>
#include <random>
#include <unordered_map>

using namespace llvm;

namespace vereda::witness {

WitnessModuleInfo extractModuleInfo(const Module &m) {
  WitnessModuleInfo info;
  info.pointerBitWidth = m.getDataLayout().getPointerSizeInBits();
  for (const DICompileUnit *cu : m.debug_compile_units()) {
    SmallString<128> path(cu->getFilename());
    sys::path::make_absolute(cu->getDirectory(), path);
    info.inputFiles.emplace_back(cu->getFilename().str(), std::string(path));
  }
  for (const Function &f : m) {
    if (!f.isDeclaration())
      continue;
    WitnessModuleInfo::NondetRet ret;
    Type *rt = f.getReturnType();
    if (rt->isPointerTy())
      ret.kind = WitnessModuleInfo::RetKind::Pointer;
    else if (rt->isHalfTy())
      ret.kind = WitnessModuleInfo::RetKind::Half;
    else if (rt->isFloatTy())
      ret.kind = WitnessModuleInfo::RetKind::Float;
    else if (rt->isDoubleTy())
      ret.kind = WitnessModuleInfo::RetKind::Double;
    else if (rt->isX86_FP80Ty())
      ret.kind = WitnessModuleInfo::RetKind::LongDouble;
    else if (rt->isFP128Ty())
      ret.kind = WitnessModuleInfo::RetKind::Float128;
    else if (rt->isIntegerTy())
      ret.kind = WitnessModuleInfo::RetKind::Integer;
    ret.hasSExt = f.hasRetAttribute(Attribute::SExt);
    ret.hasZExt = f.hasRetAttribute(Attribute::ZExt);
    info.nondetReturns.try_emplace(f.getName(), ret);
  }
  return info;
}

static void writeQuoted(raw_ostream &os, StringRef s) {
  os << '"';
  size_t start = 0;
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '"' || s[i] == '\\') {
      os << s.slice(start, i) << '\\' << s[i];
      start = i + 1;
    }
  }
  os << s.drop_front(start) << '"';
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
static std::map<std::string, Source> inputFiles(const WitnessModuleInfo &info) {
  std::map<std::string, Source> files;
  for (const auto &[name, path] : info.inputFiles) {
    if (auto buf = MemoryBuffer::getFile(path))
      files.insert_or_assign(name, Source(std::move(*buf)));
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
static std::optional<std::pair<unsigned, unsigned>> keyword(const Source &file, unsigned line,
                                                            unsigned col) {
  StringRef text = file.text();
  if (line > file.starts.size() || !col)
    return std::nullopt;
  size_t pos = file.starts[line - 1] + col - 1;
  if (pos >= text.size())
    return std::nullopt;
  auto isIdent = [](char c) { return isAlnum(c) || c == '_'; };
  StringRef word = text.drop_front(pos).take_while(isIdent);
  if (word == "while" || word == "for")
    return std::pair<unsigned, unsigned>{line, col};
  StringRef before = text.take_front(pos).rtrim();
  if (!before.consume_back("("))
    return std::nullopt;
  before = before.rtrim();
  if (!before.ends_with("if") || (before.size() > 2 && isIdent(before[before.size() - 3])))
    return std::nullopt;
  size_t start = before.size() - 2;
  auto next = std::upper_bound(file.starts.begin(), file.starts.end(), start);
  return std::pair<unsigned, unsigned>{static_cast<unsigned>(next - file.starts.begin()),
                                       static_cast<unsigned>(start - next[-1] + 1)};
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
static std::string constraint(solver::Model &model, const walker::Trace &trace,
                              const walker::Step &step, const WitnessModuleInfo &info) {
  const APInt *value = model.value(step.value);
  if (!value)
    return "";
  if (step.value.isBool()) // a _Bool
    return std::string("\\result == ") + (value->isOne() ? "1" : "0");
  const APInt &v = *value;
  const std::string &stepText = trace.text(step);
  auto it = info.nondetReturns.find(stepText);
  if (it == info.nondetReturns.end())
    return "";
  const WitnessModuleInfo::NondetRet &ret = it->second;
  // A pointer is an object and an offset, and no constant names an object;
  // \result != 0 still keeps the waypoint at its call (#76).
  if (ret.kind == WitnessModuleInfo::RetKind::Pointer)
    return v.isZero() ? "\\result == 0" : "\\result != 0";
  using Kind = WitnessModuleInfo::RetKind;
  if (ret.kind == Kind::Half || ret.kind == Kind::Float || ret.kind == Kind::Double ||
      ret.kind == Kind::LongDouble || ret.kind == Kind::Float128) {
    // No constant compares equal to a NaN, and an infinity is beyond the
    // largest finite value. The sign of a zero does not show: -0.0 == 0.0.
    const fltSemantics &sem = ret.kind == Kind::Half         ? APFloat::IEEEhalf()
                              : ret.kind == Kind::Float      ? APFloat::IEEEsingle()
                              : ret.kind == Kind::Double     ? APFloat::IEEEdouble()
                              : ret.kind == Kind::LongDouble ? APFloat::x87DoubleExtended()
                                                             : APFloat::IEEEquad();
    APInt bits = v;
    if (ret.kind == WitnessModuleInfo::RetKind::LongDouble && bits.getBitWidth() == 79) {
      // Native FP terms for x86_fp80 omit the explicit integer bit; restore it
      // (0 for zero/subnormal, 1 otherwise) for APFloat::x87DoubleExtended.
      APInt signExp = bits.extractBits(16, 63);
      APInt exp = bits.extractBits(15, 63);
      APInt intBit(1, !exp.isZero());
      bits = signExp.concat(intBit).concat(bits.extractBits(63, 0));
    }
    APFloat x(sem, bits);
    if (x.isNaN())
      return "";
    const char *suffix = ret.kind == Kind::LongDouble ? "L" : ret.kind == Kind::Float128 ? "Q" : "";
    if (x.isInfinity())
      return std::string("\\result ") + (x.isNegative() ? "< " : "> ") +
             hex(APFloat::getLargest(x.getSemantics(), x.isNegative())) + suffix;
    return "\\result == " + hex(x) + suffix;
  }
  // The ABI returns an __int128 as { i64, i64 }, whose packed bits, the
  // first field lowest, are the integer's.
  StringRef type = stepText;
  type.consume_front("__VERIFIER_nondet_");
  if (ret.kind != WitnessModuleInfo::RetKind::Integer && !signedTypes.contains(type) &&
      !unsignedTypes.contains(type))
    return "";
  // C promotes a type narrower than int before the comparison, so 200 and
  // -56 differ for a char. The name gives the sign, or else the extension
  // the ABI asks of a narrow result. A value whose top bit is clear reads
  // the same in either sign; one whose top bit is set, of a type whose
  // sign neither tells (pthread_t), gets no constraint.
  if (signedTypes.contains(type) || ret.hasSExt)
    return equals(v, true);
  if (unsignedTypes.contains(type) || ret.hasZExt || v.isNonNegative())
    return equals(v, false);
  return "";
}

static void waypoint(raw_ostream &os, StringRef type, StringRef file, unsigned line, unsigned col,
                     StringRef function, StringRef constraint) {
  os << "  - segment:\n"
     << "    - waypoint:\n"
     << "        type: ";
  writeQuoted(os, type);
  os << "\n"
     << "        action: \"follow\"\n";
  if (!constraint.empty()) {
    os << "        constraint:\n"
       << "          value: ";
    writeQuoted(os, constraint);
    os << "\n";
    if (type != "branching") // whose constraint is a value, not an expression
      os << "          format: \"acsl_expression\"\n";
  }
  os << "        location:\n"
     << "          file_name: ";
  writeQuoted(os, file);
  os << "\n"
     << "          line: " << line << "\n";
  if (col)
    os << "          column: " << col << "\n";
  os << "          function: ";
  writeQuoted(os, function);
  os << "\n";
}

Error violationWitness(solver::Model &model, const walker::Trace &trace,
                       const checks::Checks &checks, unsigned violated,
                       const WitnessModuleInfo &info, StringRef entry,
                       function_ref<void(function_ref<void(raw_ostream &)>)> emit) {
  auto files = inputFiles(info);
  // Memoize source-text lookups per site: a long loop repeats the same
  // branch and nondet call sites thousands of times.
  struct KeywordKey {
    const Source *src;
    unsigned line;
    unsigned col;
    bool operator==(const KeywordKey &) const = default;
  };
  struct KeywordHash {
    size_t operator()(const KeywordKey &k) const {
      return hash_combine(k.src, k.line, k.col);
    }
  };
  std::unordered_map<KeywordKey, std::optional<std::pair<unsigned, unsigned>>, KeywordHash>
      keywordCache;

  struct RetColKey {
    const Source *src;
    unsigned line;
    unsigned col;
    uint32_t textId;
    bool operator==(const RetColKey &) const = default;
  };
  struct RetColHash {
    size_t operator()(const RetColKey &k) const {
      return hash_combine(k.src, k.line, k.col, k.textId);
    }
  };
  std::unordered_map<RetColKey, unsigned, RetColHash> retColCache;

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
    const ir::SourceLoc &stepLoc = trace.loc(step);
    auto file = files.find(stepLoc.file);
    if (file == files.end() || !stepLoc.line)
      continue;
    if (step.kind == walker::Step::Branch) {
      auto [it, fresh] =
          keywordCache.try_emplace(KeywordKey{&file->second, stepLoc.line, stepLoc.col});
      if (fresh)
        it->second = keyword(file->second, stepLoc.line, stepLoc.col);
      if (it->second) {
        lastBranching = content.size();
        waypoint(os, "branching", stepLoc.file, it->second->first, it->second->second,
                 stepLoc.function, trace.text(step));
      }
      continue;
    }
    std::string value = constraint(model, trace, step, info);
    if (!value.empty()) {
      lastBranching = std::string::npos;
      auto [it, fresh] =
          retColCache.try_emplace(RetColKey{&file->second, stepLoc.line, stepLoc.col, step.text});
      if (fresh)
        it->second = returnColumn(file->second, stepLoc, trace.text(step));
      waypoint(os, "function_return", stepLoc.file, stepLoc.line, it->second, stepLoc.function,
               value);
    }
  }
  // The witness states unreach-call, which only a call to reach_error
  // violates.
  if (!target || trace.text(*target) != "reach_error")
    return createStringError("the violation (" + checks[violated].comment +
                             ") is not a call to reach_error");
  const ir::SourceLoc &targetLoc = trace.loc(*target);
  if (!files.count(targetLoc.file))
    return createStringError("the call to reach_error is in " + targetLoc.file +
                             ", not in an input file");
  // A branching waypoint right before the target says little the target
  // does not: mostly it is the if around the call to reach_error. With it,
  // CPAchecker 4.3's refinement fails on some tasks it confirms without it
  // (MathSAT cannot interpolate).
  if (lastBranching != std::string::npos)
    content.resize(lastBranching);
  waypoint(os, "target", targetLoc.file, targetLoc.line, targetLoc.col, targetLoc.function, "");

  char now[32];
  std::time_t t = std::time(nullptr);
  std::strftime(now, sizeof now, "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&t));
  emit([&](raw_ostream &w) {
    w << "- entry_type: \"violation_sequence\"\n"
      << "  metadata:\n"
      << "    format_version: \"2.0\"\n"
      << "    uuid: ";
    writeQuoted(w, uuid());
    w << "\n"
      << "    creation_time: ";
    writeQuoted(w, now);
    w << "\n"
      << "    producer:\n"
      << "      name: \"Vereda\"\n"
      << "      version: ";
    writeQuoted(w, VEREDA_VERSION);
    w << "\n"
      << "    task:\n"
      << "      input_files:\n";
    for (auto &[name, source] : files) {
      w << "      - ";
      writeQuoted(w, name);
      w << "\n";
    }
    w << "      input_file_hashes:\n";
    for (auto &[name, source] : files) {
      w << "        ";
      writeQuoted(w, name);
      w << ": ";
      writeQuoted(w, toHex(SHA256::hash(arrayRefFromStringRef(source.text())), /*LowerCase=*/true));
      w << "\n";
    }
    w << "      specification: ";
    writeQuoted(w, "CHECK( init(" + entry.str() + "()), LTL(G ! call(reach_error())) )");
    w << "\n"
      << "      data_model: "
      << (info.pointerBitWidth == 32 ? "\"ILP32\"" : "\"LP64\"") << "\n"
      << "      language: \"C\"\n"
      << "  content:\n"
      << content;
  });
  return Error::success();
}

} // namespace vereda::witness
