// Stage analysis: proves that every run of the entry function ends, without
// walking it (issue #38), when no function it reaches recurses and SCEV
// bounds the iterations of every loop in each one. The IR comes from ir with
// the poison flags, attributes and metadata stripped, so SCEV counts as the
// walker computes, with wrap-around; docs/survey/2026-11-27-termination-scev.md
// gives the argument.
#pragma once
#include <llvm/IR/Function.h>
#include <string>

namespace vereda::analysis {

struct Termination {
  // Why SCEV does not show that every run of entry ends; empty when it does.
  std::string unbounded;
  // Why a walk cannot show it either, as code runs that the walk does not
  // see (a callback from a bodyless function, a constructor, inline
  // assembly); empty when the walk sees every loop and call.
  std::string unseen;
};

Termination termination(llvm::Function &entry);

} // namespace vereda::analysis
