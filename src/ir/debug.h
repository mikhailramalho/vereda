// Stage ir: debug-info helpers. Source locations come from clang's -g output.
#pragma once
#include <llvm/BinaryFormat/Dwarf.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/IR/DebugLoc.h>
#include <llvm/IR/DebugProgramInstruction.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Instruction.h>
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
};

inline SourceLoc location(const llvm::DebugLoc &loc, llvm::StringRef function) {
  if (!loc)
    return {"<unknown>", 0, 0, function.str()};
  return {loc->getFilename().str(), loc.getLine(), loc.getCol(), function.str()};
}

inline SourceLoc location(const llvm::Instruction &inst) {
  return location(inst.getDebugLoc(), inst.getFunction()->getName());
}

// A #dbg_value record that assigns one whole value to a source variable.
struct DbgAssign {
  std::string name;
  const llvm::Value *value;
  bool isUnsigned;
  SourceLoc loc;
};

inline std::optional<DbgAssign> dbgAssign(const llvm::DbgRecord &rec) {
  auto *dvr = llvm::dyn_cast<llvm::DbgVariableRecord>(&rec);
  if (!dvr || !dvr->isDbgValue() || dvr->hasArgList() || dvr->getNumVariableLocationOps() != 1 ||
      dvr->getExpression()->getNumElements() != 0)
    return std::nullopt;
  const llvm::Value *v = dvr->getVariableLocationOp(0);
  if (!v)
    return std::nullopt;
  // A parameter receiving its own argument is the call, not an assignment;
  // ESBMC shows none. Another value, another parameter's included, is one.
  if (auto *arg = llvm::dyn_cast<llvm::Argument>(v);
      arg && dvr->getVariable()->getArg() == arg->getArgNo() + 1 &&
      arg->getParent() == dvr->getFunction())
    return std::nullopt;
  bool isUnsigned = false;
  if (auto *basic = llvm::dyn_cast_or_null<llvm::DIBasicType>(dvr->getVariable()->getType()))
    isUnsigned = basic->getEncoding() == llvm::dwarf::DW_ATE_unsigned ||
                 basic->getEncoding() == llvm::dwarf::DW_ATE_unsigned_char ||
                 basic->getEncoding() == llvm::dwarf::DW_ATE_boolean;
  // mem2reg leaves many records at line 0; the declaration is the next best place.
  const llvm::DILocalVariable *var = dvr->getVariable();
  llvm::StringRef function = dvr->getFunction()->getName();
  SourceLoc loc = dvr->getDebugLoc() && dvr->getDebugLoc().getLine() != 0
                      ? location(dvr->getDebugLoc(), function)
                      : SourceLoc{var->getFilename().str(), var->getLine(), 0, function.str()};
  return DbgAssign{var->getName().str(), v, isUnsigned, loc};
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
// an array whose length is not constant. A union member is the first whose
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
          if (!count) // a variable-length or open dimension
            return std::nullopt;
          counts.push_back(count->getZExtValue());
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
    // A scalar: the store must write all of it.
    if (off != 0 || ty->getSizeInBits() != size * 8)
      return std::nullopt;
    if (auto *b = llvm::dyn_cast<llvm::DIBasicType>(ty))
      isUnsigned = b->getEncoding() == llvm::dwarf::DW_ATE_unsigned ||
                   b->getEncoding() == llvm::dwarf::DW_ATE_unsigned_char ||
                   b->getEncoding() == llvm::dwarf::DW_ATE_boolean;
    return path;
  }
  return std::nullopt;
}

} // namespace vereda::ir
