// Stage ir: what a sanitizer check that clang compiled to
// llvm.ubsantrap(i8 code) was checking. The code is clang's SanitizerHandler
// index (clang/lib/CodeGen/SanitizerHandler.h), identical in LLVM 21, 22
// and 23; recheck the list when a new LLVM version is supported.
#pragma once
#include <string>

namespace vereda::ir {

inline std::string sanitizerCheck(unsigned code) {
  static const char *const names[] = {
      "arithmetic overflow on add",         // AddOverflow
      "__builtin_unreachable",              // BuiltinUnreachable
      "control-flow integrity",             // CFICheckFail
      "division by zero or overflow",       // DivremOverflow
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
      "shift out of bounds",                // ShiftOutOfBounds
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

} // namespace vereda::ir
