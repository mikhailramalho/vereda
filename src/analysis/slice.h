// Stage analysis: a static slice of the rolled IR before the walk (#115).
// It removes the instructions whose values reach no check, no branch, no
// call and no memory that a kept load may read, so that the walker does not
// compute them. Every terminator, call (with its arguments), side effect,
// volatile access and variable-size alloca stays, every loop and every
// function a loop calls stays whole, and so does every store into an object
// whose address escapes (to memory, a call, an integer, a comparison with
// another address or a return), or that a kept load may read. That is all a
// check or a path guard can depend on while loads and stores carry no check
// of their own but the uninitialised read (keepLoads, which keeps them all)
// and the write into a constant object (constant globals are never
// sliced). A dereference check (#29) would make every access a root.
#pragma once
#include <llvm/IR/Module.h>

namespace vereda::analysis {

// keepLoads: every load is a check (--ub-uninit-check), and a store decides
// whether the load it copies is checked. keepNondets: a violation witness
// records the value of every __VERIFIER_nondet_ call. Returns the number of
// instructions removed.
unsigned slice(llvm::Module &m, bool keepLoads, bool keepNondets);

} // namespace vereda::analysis
