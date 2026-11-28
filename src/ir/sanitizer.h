// Stage ir: decodes clang sanitizer trap codes, integer operand descriptors
// and the comments load() gives the checks.
// Trap codes are SanitizerHandler indices (clang/lib/CodeGen/SanitizerHandler.h)
// as of LLVM 23; recheck the codes and descriptor ABI when upgrading LLVM.
#pragma once
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InstrTypes.h>
#include <llvm/IR/Metadata.h>
#include <llvm/Support/Endian.h>
#include <string>

namespace vereda::ir {

// DivremOverflowData points to an integer (kind 0) or BitInt (kind 2)
// TypeDescriptor. TypeInfo encodes a power-of-two width above its signedness
// bit; signed BitInt also records its exact width. Wide ValueHandles point to storage.
inline unsigned divremOperandBits(const llvm::CallBase &call) {
  auto initialiser = [](const llvm::Value *v) -> const llvm::ConstantStruct * {
    auto *g = llvm::dyn_cast<llvm::GlobalVariable>(v->stripPointerCasts());
    return g && g->hasInitializer() ? llvm::dyn_cast<llvm::ConstantStruct>(g->getInitializer())
                                   : nullptr;
  };
  const auto *data = initialiser(call.getArgOperand(0));
  const auto *type = data && data->getNumOperands() >= 2 ? initialiser(data->getOperand(1)) : nullptr;
  if (!type || type->getNumOperands() < 2)
    return 0;
  auto *kind = llvm::dyn_cast<llvm::ConstantInt>(type->getOperand(0));
  auto *info = llvm::dyn_cast<llvm::ConstantInt>(type->getOperand(1));
  if (!kind || !(kind->isZero() || kind->equalsInt(2)) ||
      !info || info->getValue().getActiveBits() > 16 ||
      (info->getZExtValue() >> 1) > 23)
    return 0;
  if (kind->equalsInt(2) && (info->getZExtValue() & 1)) {
    auto *name = type->getNumOperands() >= 3
                     ? llvm::dyn_cast<llvm::ConstantDataArray>(type->getOperand(2)) : nullptr;
    if (!name || !name->isString())
      return 0;
    llvm::StringRef bytes = name->getRawDataValues();
    size_t end = bytes.find('\0');
    if (end == llvm::StringRef::npos || bytes.size() - end < 5)
      return 0;
    // Signed BitInt descriptors append the exact width after the name.
    unsigned width = llvm::support::endian::read32le(bytes.data() + end + 1);
    return width && width <= llvm::IntegerType::MAX_INT_BITS ? width : 0;
  }
  return 1u << (info->getZExtValue() >> 1);
}

inline std::string sanitizerCheck(unsigned code) {
  static const char *const names[] = {
      "arithmetic overflow on add",         // AddOverflow
      "__builtin_unreachable",              // BuiltinUnreachable
      "control-flow integrity",             // CFICheckFail
      "arithmetic overflow on division",    // DivremOverflow
      "dynamic type mismatch",              // DynamicTypeCacheMiss
      "float-to-integer conversion overflow", // FloatCastOverflow
      "function type mismatch",             // FunctionTypeMismatch
      "implicit conversion changes value",  // ImplicitConversion
      "invalid builtin argument",           // InvalidBuiltin
      "invalid Objective-C cast",           // InvalidObjCCast
      "load of invalid value",              // LoadInvalidValue
      "missing return",                     // MissingReturn
      "arithmetic overflow on mul",         // MulOverflow
      "arithmetic overflow on neg",         // NegateOverflow
      "null passed as nonnull argument",    // NullabilityArg
      "null returned as nonnull",           // NullabilityReturn
      "null passed as nonnull argument",    // NonnullArg
      "null returned as nonnull",           // NonnullReturn
      "array bounds violated",              // OutOfBounds
      "pointer arithmetic overflow",        // PointerOverflow
      "undefined behavior on shift operation", // ShiftOutOfBounds
      "arithmetic overflow on sub",         // SubOverflow
      "type mismatch",                      // TypeMismatch
      "alignment assumption violated",      // AlignmentAssumption
      "VLA bound not positive",             // VLABoundNotPositive
      "bounds-safety check",                // BoundsSafety
  };
  if (code < sizeof(names) / sizeof(names[0]))
    return names[code];
  return "sanitizer check " + std::to_string(code);
}

// ESBMC's comment for a division by zero, which load() gives a division
// trap that checks only the divisor (#514), as the walker's own check.
inline constexpr const char *DivisionByZero = "division by zero";

// ESBMC's comment for a sanitizer check, which load() attaches as vereda.check
// metadata where it finds the operation the check guards, else fallback.
inline std::string checkComment(const llvm::Instruction &check, std::string fallback) {
  if (auto *md = check.getMetadata("vereda.check"))
    return llvm::cast<llvm::MDString>(md->getOperand(0))->getString().str();
  return fallback;
}

} // namespace vereda::ir
