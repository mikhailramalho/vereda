// Stage analysis: the objects one iteration of a loop may write, which the
// loop transition relation (walker.h, Transition) gives nondet contents in
// its pre-state while every other object keeps what the loop finds there
// (survey 2026-11-27-loop-transition-relation).
#pragma once
#include <llvm/ADT/SetVector.h>
#include <llvm/Analysis/LoopInfo.h>

namespace vereda::analysis {

// The globals, and the allocas of l's function from before l, that an
// iteration of l or a function it calls may write: through a store, an
// atomic, a memset/memcpy/memmove destination, or a pointer argument of a
// bodyless call. An alloca in l or in a callee is new in each iteration or
// call, so none is listed. Throws Unsupported for a write through a pointer
// into any other object (an argument, a pointer loaded from memory, a heap
// object, an address made from an integer) and for a call without a body
// that may write memory but is not one the walker models as writing nothing
// or only through its first argument.
llvm::SmallSetVector<const llvm::Value *, 4> writtenObjects(const llvm::Loop &l);

} // namespace vereda::analysis
