#include "ir/source.h"
#include "ir/debug.h"

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/MemoryBuffer.h>
#include <cctype>
#include <cstdint>

using namespace llvm;

namespace vereda::ir {

struct CachedFile {
  std::unique_ptr<MemoryBuffer> buf;
  DenseMap<uint64_t, std::string> callArgs;
};

// Whole files, cached; an unreadable file caches as empty.
static CachedFile &cachedFile(StringRef path) {
  static StringMap<CachedFile> cache;
  auto [it, inserted] = cache.try_emplace(path);
  if (inserted)
    if (auto buf = MemoryBuffer::getFile(path))
      it->second.buf = std::move(*buf);
  return it->second;
}

std::string callArgumentText(const Instruction &inst) {
  const DILocation *loc = inst.getDebugLoc();
  if (!loc || loc->getLine() == 0 || loc->getColumn() == 0)
    return "";
  CachedFile &file = cachedFile(joinedPath(loc->getDirectory(), loc->getFilename()));
  uint64_t key = (static_cast<uint64_t>(loc->getLine()) << 32) | loc->getColumn();
  auto [it, inserted] = file.callArgs.try_emplace(key);
  if (!inserted)
    return it->second;
  StringRef text = file.buf ? file.buf->getBuffer() : StringRef();

  // Offset of the call's line and column; the column points at the callee.
  size_t pos = 0;
  for (unsigned line = 1; line < loc->getLine(); ++line) {
    pos = text.find('\n', pos);
    if (pos == StringRef::npos)
      return "";
    ++pos;
  }
  pos += loc->getColumn() - 1;
  size_t open = text.find('(', pos);
  if (open == StringRef::npos)
    return "";

  // Matching parenthesis; the call may span several lines.
  std::string out;
  int depth = 1;
  for (size_t i = open + 1; i < text.size(); ++i) {
    char c = text[i];
    depth += c == '(' ? 1 : c == ')' ? -1 : 0;
    if (depth == 0) {
      it->second = StringRef(out).trim().str();
      return it->second;
    }
    if (std::isspace(static_cast<unsigned char>(c))) {
      if (!out.empty() && out.back() != ' ')
        out += ' ';
    } else {
      out += c;
    }
  }
  return "";
}

} // namespace vereda::ir
