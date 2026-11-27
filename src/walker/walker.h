// Stage walker: symbolic execution over the rolled IR. Loops are unrolled on
// the fly and calls are walked in their own frame; nothing is copied.
#pragma once
#include "checks/checks.h"
#include "encode/encoder.h"
#include "memory/memory.h"
#include "walker/trace.h"
#include <llvm/IR/Function.h>
#include <llvm/Support/Error.h>
#include <memory>

namespace vereda::walker {

// Which failures count: every check, or only those of one SV-COMP
// property. A failure that does not count ends its path, as the abort it is.
enum class Property {
  All,
  UnreachCall, // a call to reach_error, whose body is not walked
  NoOverflow,  // a clang sanitizer check
  ValidFree,   // a free of a pointer no allocator returned, or already freed
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
  // A write into a string literal or an object defined const, and an
  // invalid free, are violations, as ESBMC's pointer check has them (off
  // with --no-pointer-check).
  bool pointerCheck = true;
  // Report stack objects above PTRDIFF_MAX, unless --no-vla-size-check.
  bool vlaSizeCheck = true;
  bool assertions = true; // user assertions, independent of unwinding and pointer checks
  bool multiProperty = false; // keep exploring after predicate assertion failures
  bool uninitCheck = false;   // check reads of uninitialised local/heap memory
  // Maximum SCEV constant max backedge-taken count for which a non-summarised
  // loop unrolls to completion in one walk (and skips per-iteration solver
  // feasibility queries), 0 when explicit --unwind N is active.
  unsigned maxBoundedUnroll = 0;
};

// Walks entry and everything it calls, binding values in the encoder and
// appending checks and trace steps. Returns an error for IR it cannot
// handle yet.
llvm::Error walk(llvm::Function &entry, const Options &opts, encode::Encoder &enc,
                 memory::Memory &mem, checks::Checks &checks, Trace &trace);

// Keeps a prefix stopped at the first loop or recursive call the bound cuts,
// in the entry function or a callee, at any nesting depth, and rolls back the
// suffix walked after it between bounds. A walk starts afresh at the next bound
// when that suffix allocates objects or adds global solver constraints, or when
// the first cut could not be saved (inside an array loop summary, say).
class Incremental {
public:
  Incremental(llvm::Function &entry, const Options &opts, encode::Encoder &enc,
              memory::Memory &mem, checks::Checks &checks, Trace &trace);
  ~Incremental();
  // Bounds must increase while the walk is resumable.
  llvm::Error advance(unsigned bound);
  bool resumable() const;

private:
  struct Impl;
  std::unique_ptr<Impl> impl;
};

} // namespace vereda::walker
