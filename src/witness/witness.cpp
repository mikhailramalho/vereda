#include "witness/witness.h"

#include <llvm/ADT/StringExtras.h>
#include <llvm/ADT/StringSet.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/SHA256.h>
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

// The task's source files with their contents, by the name debug
// locations use; a file that cannot be read is left out.
static std::map<std::string, std::unique_ptr<MemoryBuffer>> inputFiles(const Module &m) {
  std::map<std::string, std::unique_ptr<MemoryBuffer>> files;
  for (const DICompileUnit *cu : m.debug_compile_units()) {
    SmallString<128> path(cu->getFilename());
    sys::fs::make_absolute(cu->getDirectory(), path);
    if (auto buf = MemoryBuffer::getFile(path))
      files[cu->getFilename().str()] = std::move(*buf);
  }
  return files;
}

// The column of the ')' that closes the call to callee at loc, which is
// where a function_return waypoint points; 0 when the source there is not
// that call on one line, as for a call a macro expands to, whose location
// is the macro's name.
static unsigned closingParen(const MemoryBuffer &file, const ir::SourceLoc &loc,
                             StringRef callee) {
  SmallVector<StringRef, 0> lines;
  file.getBuffer().split(lines, '\n');
  if (loc.line > lines.size() || !loc.col)
    return 0;
  StringRef line = lines[loc.line - 1];
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

// The SV-COMP nondet functions whose sign C fixes; pthread_t, sector_t and
// the like depend on the program's typedefs.
static const StringSet<> signedTypes = {"char", "short", "int", "long", "longlong", "loff_t"};
static const StringSet<> unsignedTypes = {"uchar", "ushort", "uint",     "unsigned", "ulong",
                                          "ulonglong", "u8",  "u16",     "u32",      "u64",
                                          "size_t"};

// The model's value of a call step as a C constant, or empty when its sign
// is not known: a bodyless function has no debug type to tell, and C
// promotes a type narrower than int before the comparison, so 200 and -56
// differ for a char.
static std::string constant(camada::SMTSolver &s, const walker::Step &step) {
  StringRef type = step.text;
  type.consume_front("__VERIFIER_nondet_");
  if (type == "bool" && step.value->isBoolSort()) {
    auto b = s.getBool(step.value);
    return b ? (b.value() ? "1" : "0") : "";
  }
  if (step.value->isBoolSort() || step.value->getWidth() > 64)
    return "";
  if (unsignedTypes.contains(type)) {
    auto v = s.getBVUnsigned(step.value);
    // A decimal constant above INT64_MAX needs the suffix to have a type.
    return v ? std::to_string(v.value()) + (v.value() > INT64_MAX ? "U" : "") : "";
  }
  if (!signedTypes.contains(type))
    return "";
  auto v = s.getBV(step.value);
  // The lowest value has no decimal constant: its magnitude does not fit.
  return v && v.value() != INT64_MIN ? std::to_string(v.value()) : "";
}

static void waypoint(raw_ostream &os, StringRef type, const ir::SourceLoc &loc, unsigned col,
                     StringRef constraint) {
  os << "  - segment:\n"
     << "    - waypoint:\n"
     << "        type: " << quote(type) << "\n"
     << "        action: \"follow\"\n";
  if (!constraint.empty())
    os << "        constraint:\n"
       << "          value: " << quote(constraint) << "\n"
       << "          format: \"acsl_expression\"\n";
  os << "        location:\n"
     << "          file_name: " << quote(loc.file) << "\n"
     << "          line: " << loc.line << "\n";
  if (col)
    os << "          column: " << col << "\n";
  os << "          function: " << quote(loc.function) << "\n";
}

Expected<std::string> violationWitness(camada::SMTSolver &s, const walker::Trace &trace,
                                       const checks::Checks &checks, unsigned violated,
                                       const Module &m, StringRef entry) {
  auto files = inputFiles(m);
  // A waypoint must point into an input file: a call in a header or in a
  // library model gets none, and so does a value with no C constant.
  std::string content;
  raw_string_ostream os(content);
  const walker::Step *target = nullptr;
  for (const walker::Step &step : trace) {
    bool isCheck = step.kind == walker::Step::Check;
    if ((isCheck && step.check != violated) || (!isCheck && step.kind != walker::Step::Call))
      continue;
    auto reached = s.getBool(step.guard);
    if (!reached || !reached.value())
      continue;
    if (isCheck) {
      target = &step;
      break;
    }
    auto file = files.find(step.loc.file);
    std::string value = constant(s, step);
    if (file == files.end() || !step.loc.line || value.empty())
      continue;
    waypoint(os, "function_return", step.loc, closingParen(*file->second, step.loc, step.text),
             "\\result == " + value);
  }
  // The witness states unreach-call, which only a call to reach_error
  // violates.
  if (!target || target->text != "reach_error")
    return createStringError("the violation (" + checks[violated].comment +
                             ") is not a call to reach_error");
  if (!files.count(target->loc.file))
    return createStringError("the call to reach_error is in " + target->loc.file +
                             ", not in an input file");
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
  for (auto &[name, buf] : files)
    w << "      - " << quote(name) << "\n";
  w << "      input_file_hashes:\n";
  for (auto &[name, buf] : files)
    w << "        " << quote(name) << ": "
      << quote(toHex(SHA256::hash(arrayRefFromStringRef(buf->getBuffer())), /*LowerCase=*/true))
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
