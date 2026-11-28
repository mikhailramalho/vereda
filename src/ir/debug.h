// Stage ir: debug-info helpers. Source locations come from clang's -g output.
#pragma once
#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/BinaryFormat/Dwarf.h>
#include <llvm/Demangle/Demangle.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugProgramInstruction.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/Path.h>
#include <optional>
#include <string>

namespace vereda::ir {

// A source location in ESBMC's spelling: "file F line L column C function Fn".
struct SourceLoc {
  std::string file;
  unsigned line = 0;
  unsigned col = 0;
  std::string function;

  std::string str() const {
    std::string out = "file " + file + " line " + std::to_string(line);
    if (col)
      out += " column " + std::to_string(col);
    return out + " function " + function;
  }

  std::string display() const {
    SourceLoc loc = *this;
    loc.function = llvm::demangle(function);
    if (loc.file.empty() || loc.file == "<unknown>")
      return "function " + loc.function;
    return loc.str();
  }
};

// name in directory dir, as one path.
inline std::string joinedPath(llvm::StringRef dir, llvm::StringRef name) {
  llvm::SmallString<256> path;
  if (!llvm::sys::path::is_absolute(name))
    path = dir;
  llvm::sys::path::append(path, name);
  return std::string(path);
}

// A file's name in locations: the compile unit's own spelling when it is the
// unit's main file, as the user named it, else its full path. clang may split
// a path that shares a prefix with its working directory into that prefix
// and a relative rest (#85), so the names are compared joined. Built once per
// file and unit, as the walk asks again at every step: the metadata is
// uniqued and lives as long as the driver's one LLVMContext.
inline std::string fileName(const llvm::DIFile *file, const llvm::DILocalScope *scope) {
  const llvm::DISubprogram *sp = scope->getSubprogram();
  const llvm::DICompileUnit *cu = sp ? sp->getUnit() : nullptr;
  static llvm::DenseMap<std::pair<const llvm::DIFile *, const llvm::DICompileUnit *>, std::string>
      cache;
  auto [it, inserted] = cache.try_emplace({file, cu});
  if (inserted) {
    std::string path = file ? joinedPath(file->getDirectory(), file->getFilename()) : "";
    it->second = cu && path == joinedPath(cu->getDirectory(), cu->getFilename())
                     ? cu->getFilename().str()
                     : std::move(path);
  }
  return it->second;
}

inline SourceLoc location(const llvm::DebugLoc &loc, llvm::StringRef function) {
  if (!loc)
    return {"<unknown>", 0, 0, function.str()};
  return {fileName(loc->getFile(), loc->getScope()), loc.getLine(), loc.getCol(), function.str()};
}

inline SourceLoc location(const llvm::Instruction &inst) {
  return location(inst.getDebugLoc(), inst.getFunction()->getName());
}

inline std::optional<std::string> memberPath(const llvm::DIType *ty, uint64_t off, uint64_t size,
                                             bool &isUnsigned);

// A #dbg_value record that assigns one whole value to a source variable, or
// to the member of one that a fragment covers (name is then its path).
struct DbgAssign {
  std::string name;
  const llvm::Value *value;
  bool isUnsigned;
  SourceLoc loc;
};

inline std::optional<DbgAssign> dbgAssign(const llvm::DbgRecord &rec) {
  auto *dvr = llvm::dyn_cast<llvm::DbgVariableRecord>(&rec);
  if (!dvr || !dvr->isDbgValue() || dvr->hasArgList() || dvr->getNumVariableLocationOps() != 1)
    return std::nullopt;
  // Nothing but a fragment, as SROA leaves for each member of an aggregate
  // it splits (#311).
  std::optional<llvm::DIExpression::FragmentInfo> fragment =
      dvr->getExpression()->getFragmentInfo();
  if (dvr->getExpression()->getNumElements() != (fragment ? 3 : 0))
    return std::nullopt;
  const llvm::Value *v = dvr->getVariableLocationOp(0);
  if (!v)
    return std::nullopt;
  // A value a pass removed is poison: no assignment.
  if (llvm::isa<llvm::UndefValue>(v))
    return std::nullopt;
  // A parameter receiving its own argument is the call, not an assignment;
  // ESBMC shows none. Another value, another parameter's included, is one.
  if (auto *arg = llvm::dyn_cast<llvm::Argument>(v);
      arg && dvr->getVariable()->getArg() == arg->getArgNo() + 1 &&
      arg->getParent() == dvr->getFunction())
    return std::nullopt;
  bool isUnsigned = false;
  std::string name = dvr->getVariable()->getName().str();
  if (fragment) { // the member the fragment covers, as a store to it shows
    if (fragment->OffsetInBits % 8 || fragment->SizeInBits % 8)
      return std::nullopt;
    std::optional<std::string> path = memberPath(dvr->getVariable()->getType(),
                                                 fragment->OffsetInBits / 8,
                                                 fragment->SizeInBits / 8, isUnsigned);
    if (!path)
      return std::nullopt;
    name += *path;
  } else if (auto *basic =
                 llvm::dyn_cast_or_null<llvm::DIBasicType>(dvr->getVariable()->getType())) {
    isUnsigned = basic->getEncoding() == llvm::dwarf::DW_ATE_unsigned ||
                 basic->getEncoding() == llvm::dwarf::DW_ATE_unsigned_char ||
                 basic->getEncoding() == llvm::dwarf::DW_ATE_boolean;
  }
  // mem2reg leaves many records at line 0; the declaration is the next best place.
  const llvm::DILocalVariable *var = dvr->getVariable();
  llvm::StringRef function = dvr->getFunction()->getName();
  SourceLoc loc = dvr->getDebugLoc() && dvr->getDebugLoc().getLine() != 0
                      ? location(dvr->getDebugLoc(), function)
                      : SourceLoc{fileName(var->getFile(), var->getScope()), var->getLine(), 0,
                                  function.str()};
  return DbgAssign{std::move(name), v, isUnsigned, std::move(loc)};
}

// A conditional branch clang made for the condition of an if, while or for
// statement, read from the names it gives the blocks (kept by
// -fno-discard-value-names; InstCombine may swap the successors, and loop
// simplification may split them, but the names stay): the value of the
// whole condition when the branch takes each successor. A branch on an
// operand of && or || that goes on to the next operand (land.lhs.true,
// lor.lhs.false) decides nothing there. loc is the keyword of a loop; for
// an if, the start of its condition, where clang opens the if's lexical
// block. A do-while is left out: CPAchecker 4.3 refutes a witness with a
// branching waypoint for one, at its do or its while.
struct BranchSite {
  SourceLoc loc;
  std::optional<bool> taken[2];
};

inline std::optional<BranchSite> branchSite(const llvm::CondBrInst &br) {
  static const std::pair<llvm::StringRef, bool> blocks[] = {
      {"if.then", true},    {"if.else", false}, {"if.end", false}, {"while.body", true},
      {"while.end", false}, {"for.body", true}, {"for.end", false}};
  BranchSite site;
  bool isIf = false;
  for (unsigned i = 0; i < 2; ++i)
    for (auto &[name, value] : blocks)
      if (br.getSuccessor(i)->getName().starts_with(name)) {
        site.taken[i] = value;
        isIf = name.starts_with("if.");
      }
  const llvm::DILocation *loc = br.getDebugLoc();
  if ((!site.taken[0] && !site.taken[1]) || !loc)
    return std::nullopt;
  site.loc = location(br.getDebugLoc(), br.getFunction()->getName());
  if (isIf) {
    auto *block = llvm::dyn_cast<llvm::DILexicalBlock>(loc->getScope());
    if (!block)
      return std::nullopt;
    site.loc.line = block->getLine();
    site.loc.col = block->getColumn();
  }
  return site;
}

// ty without typedefs and qualifiers.
inline const llvm::DIType *stripTypedefs(const llvm::DIType *ty) {
  while (auto *d = llvm::dyn_cast_or_null<llvm::DIDerivedType>(ty)) {
    unsigned tag = d->getTag();
    if (tag != llvm::dwarf::DW_TAG_typedef && tag != llvm::dwarf::DW_TAG_const_type &&
        tag != llvm::dwarf::DW_TAG_volatile_type && tag != llvm::dwarf::DW_TAG_restrict_type &&
        tag != llvm::dwarf::DW_TAG_atomic_type)
      break;
    ty = d->getBaseType();
  }
  return ty;
}

// The path from a variable of debug type ty to the scalar a store of size
// bytes at byte offset off writes, as C spells it ("[2][0].x"), and whether
// that scalar is unsigned. None when no single scalar of that size is there:
// a bitfield (its store writes the whole unit), a partial or spanning write,
// an array whose length is not constant but in its outermost dimension (a
// VLA's, which no index needs). A union member is the first whose
// size the store matches: members of one size cannot be told apart, as clang
// addresses them all through the first.
inline std::optional<std::string> memberPath(const llvm::DIType *ty, uint64_t off, uint64_t size,
                                             bool &isUnsigned) {
  std::string path;
  isUnsigned = false;
  for (ty = stripTypedefs(ty); ty; ty = stripTypedefs(ty)) {
    auto *c = llvm::dyn_cast<llvm::DICompositeType>(ty);
    if (c && c->getTag() == llvm::dwarf::DW_TAG_array_type) {
      const llvm::DIType *elem = stripTypedefs(c->getBaseType());
      uint64_t stride = elem ? elem->getSizeInBits() / 8 : 0;
      llvm::SmallVector<uint64_t, 4> counts; // one per dimension, outermost first
      for (llvm::DINode *n : c->getElements())
        if (auto *r = llvm::dyn_cast<llvm::DISubrange>(n)) {
          auto *count = llvm::dyn_cast_if_present<llvm::ConstantInt *>(r->getCount());
          if (!count && !counts.empty()) // a variable-length inner dimension
            return std::nullopt;
          counts.push_back(count ? count->getZExtValue() : 0);
        }
      if (!stride || counts.empty())
        return std::nullopt;
      for (size_t d = 0; d < counts.size(); ++d) {
        uint64_t inner = stride;
        for (size_t e = d + 1; e < counts.size(); ++e)
          inner *= counts[e];
        path += "[" + std::to_string(off / inner) + "]";
        off %= inner;
      }
      ty = elem;
      continue;
    }
    if (c && (c->getTag() == llvm::dwarf::DW_TAG_structure_type ||
              c->getTag() == llvm::dwarf::DW_TAG_class_type ||
              c->getTag() == llvm::dwarf::DW_TAG_union_type)) {
      const llvm::DIDerivedType *member = nullptr;
      bool exact = false;
      for (llvm::DINode *n : c->getElements()) {
        auto *m = llvm::dyn_cast<llvm::DIDerivedType>(n);
        if (!m || m->getTag() != llvm::dwarf::DW_TAG_member || m->isStaticMember())
          continue;
        uint64_t start = m->getOffsetInBits() / 8, bytes = (m->getSizeInBits() + 7) / 8;
        if (start > off || off + size > start + bytes)
          continue;
        if (m->isBitField())
          return std::nullopt;
        // Prefer the first member the store fills exactly (a union's int
        // over its char).
        bool fills = start == off && bytes == size;
        if (!member || (fills && !exact)) {
          member = m;
          exact = fills;
        }
      }
      if (!member)
        return std::nullopt;
      path += "." + member->getName().str();
      off -= member->getOffsetInBits() / 8;
      ty = member->getBaseType();
      continue;
    }
    // A scalar: the store must write all of it, except that an x86 long
    // double's 10 bytes fill the 12 or 16 of its type.
    auto *b = llvm::dyn_cast<llvm::DIBasicType>(ty);
    bool x87 = b && b->getEncoding() == llvm::dwarf::DW_ATE_float && size == 10;
    if (off != 0 || (ty->getSizeInBits() != size * 8 && !x87))
      return std::nullopt;
    if (b)
      isUnsigned = b->getEncoding() == llvm::dwarf::DW_ATE_unsigned ||
                   b->getEncoding() == llvm::dwarf::DW_ATE_unsigned_char ||
                   b->getEncoding() == llvm::dwarf::DW_ATE_boolean;
    return path;
  }
  return std::nullopt;
}

} // namespace vereda::ir
