// Stage ir: loads bitcode or textual IR and prepares it for the walker.
#pragma once
#include <llvm/IR/Module.h>
#include <llvm/Support/Error.h>
#include <memory>

namespace vereda::ir {

// Rejects mustprogress, strips the flags, attributes and metadata that let
// LLVM assume no undefined behaviour, and runs the short pass list from the
// plan (section 4.1): mem2reg, SROA, InstCombine, loop-simplify, LCSSA.
// Indirect calls become tests of the pointer and direct calls, and direct
// calls to a function of another type convert their arguments.
// setjmp and longjmp become control flow. Then links in the library models
// (models-<pointer bits>.bc in models, a directory; none when empty or
// missing) for the functions the module still calls without defining.
// With divByZeroCheck, preserves integer divisor checks before optimisation.
llvm::Expected<std::unique_ptr<llvm::Module>> load(llvm::StringRef path, llvm::LLVMContext &ctx,
                                                   llvm::StringRef models = "",
                                                   llvm::StringRef entry = "",
                                                   bool divByZeroCheck = true);

} // namespace vereda::ir
