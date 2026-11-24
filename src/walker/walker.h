// Stage walker: symbolic execution over the rolled IR. Loops are unrolled on
// the fly and calls are walked in their own frame; nothing is copied.
#pragma once
#include "checks/checks.h"
#include "encode/encoder.h"
#include "memory/memory.h"
#include "walker/trace.h"
#include <llvm/IR/Function.h>
#include <llvm/Support/Error.h>

namespace vereda::walker {

// Which failures count: every check, or only those of one SV-COMP
// property. A failure that does not count ends its path, as the abort it is.
enum class Property {
  All,
  UnreachCall, // a call to reach_error, whose body is not walked
  NoOverflow,  // a clang sanitizer check
};

struct Options {
  unsigned unwind = 0;             // loop iterations and recursion depth; 0 = until unreachable
  bool unwindingAssertions = true; // check the bound instead of assuming it
  bool forceMallocSuccess = false; // malloc never returns null
  Property property = Property::All;
  // An "Unwinding loop" line per iteration; off under --incremental-bmc,
  // whose walks repeat every iteration at each bound, so an unbounded run
  // printed gigabytes.
  bool showIterations = true;
  // The loops analysis/loops.h accepts are replaced by their effect, both
  // those with a store (ESBMC's default) and those that only check or count
  // (its --summarize-array-loops).
  bool summarise = true;
  // A violation witness is asked for. It needs a waypoint per nondet value,
  // which a summarised iteration gives for one index only, so loops that
  // call nondet are unrolled.
  bool witness = false;
};

// Walks entry and everything it calls, binding values in the encoder and
// appending checks and trace steps. Returns an error for IR it cannot
// handle yet.
llvm::Error walk(llvm::Function &entry, const Options &opts, encode::Encoder &enc,
                 memory::Memory &mem, checks::Checks &checks, Trace &trace);

} // namespace vereda::walker
