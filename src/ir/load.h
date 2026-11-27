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
// With countZeroCheck, checks poison-on-zero count intrinsics before optimisation.
// With floatCastOverflowCheck, checks out-of-range fptosi/fptoui before optimisation.
// With alignmentCheck, preserves typed memory alignment checks before InstCombine.
// With uninitCheck, keeps uninitialised locals in memory for read checking.
// With errorLabel, reaching a C label of that name fails a check.
llvm::Expected<std::unique_ptr<llvm::Module>> load(llvm::StringRef path, llvm::LLVMContext &ctx,
                                                   llvm::StringRef models = "",
                                                   llvm::StringRef entry = "",
                                                   bool divByZeroCheck = true,
                                                   bool countZeroCheck = false,
                                                   bool floatCastOverflowCheck = false,
                                                   bool alignmentCheck = false,
                                                   bool uninitCheck = false,
                                                   llvm::StringRef errorLabel = "");

// Whether the loader runs code g names, outside the entry's calls: a
// constructor or destructor list, a global placed in one of their sections,
// or an ifunc, whose resolver runs.
bool runsAtLoad(const llvm::GlobalValue &g);

} // namespace vereda::ir
