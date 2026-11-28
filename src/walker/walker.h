// Stage walker: symbolic execution over the rolled IR. Loops are unrolled on
// the fly and calls are walked in their own frame; nothing is copied.
#pragma once
#include "checks/checks.h"
#include "encode/encoder.h"
#include "memory/memory.h"
#include "walker/trace.h"
#include <llvm/IR/Function.h>
#include <llvm/Support/Error.h>
#include <map>
#include <memory>
#include <vector>

namespace vereda::walker {

// Which failures count: every check, or only those of one SV-COMP
// property. A failure that does not count ends its path, as the abort it is.
enum class Property {
  All,
  UnreachCall, // a call to reach_error, whose body is not walked
  NoOverflow,  // a clang sanitizer check
  ValidFree,   // a free of a pointer no allocator returned, or already freed
  // No failure: a path ends only by ending the run, and the unwinding and
  // recursion checks say whether the bound left some run unfinished.
  Termination,
};

struct Options {
  unsigned unwind = 0;             // loop iterations and recursion depth; 0 = until unreachable
  bool unwindingAssertions = true; // check the bound instead of assuming it
  bool forceMallocSuccess = false; // malloc never returns null
  Property property = Property::All;
  // An "Unwinding loop" line the first time a loop reaches each iteration,
  // and past iteration 1024 only at powers of two, so that a loop walked
  // again or a long loop does not flood the output (#383). Off under
  // --incremental-bmc, whose walks repeat every iteration at each bound.
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
  // with --ub-pointer-check=false).
  bool pointerCheck = true;
  // Report stack objects above PTRDIFF_MAX, unless --vla-size-check=false.
  bool vlaSizeCheck = true;
  bool assertions = true; // user assertions, independent of unwinding and pointer checks
  bool multiProperty = false; // keep exploring after predicate assertion failures
  bool uninitCheck = false;   // check reads of uninitialised local/heap memory
  // Maximum SCEV constant max backedge-taken count for which a non-summarised
  // loop unrolls to completion in one walk (and skips per-iteration solver
  // feasibility queries), 0 when explicit --unwind N is active.
  unsigned maxBoundedUnroll = 0;
  // --unwindsetname: the bound of some loops, by function and loop number,
  // in place of unwind.
  std::map<std::pair<std::string, unsigned>, unsigned> loopBounds;
  // A #pragma unroll bounds its loop. Off under --incremental-bmc, whose
  // bound grows from walk to walk and a fixed one would never let it end.
  bool pragmaBounds = true;
};

// Walks entry and everything it calls, binding values in the encoder and
// appending checks and trace steps. Returns an error for IR it cannot
// handle yet.
llvm::Error walk(llvm::Function &entry, const Options &opts, encode::Encoder &enc,
                 memory::Memory &mem, checks::Checks &checks, Trace &trace);

// One pass through a loop's body from any state at its header (plan, §4.5
// choice 4; survey 2026-11-27-loop-transition-relation). The post-state is
// built from the pre-state's symbols, so next with post = f(pre) is the
// relation.
struct Transition {
  // Each header phi a fresh value, in order, and memory as the loop is
  // entered but with every object the loop may write given nondet contents
  // (analysis/writes.h).
  std::vector<encode::Term> pre;
  memory::State preMem;
  // Where the iteration takes a back edge, and the phis and memory there.
  encode::Term next;
  std::vector<encode::Term> post;
  memory::State postMem;
  // The checks the iteration raised: an index into checks, and what
  // violates it in this iteration.
  std::vector<std::pair<size_t, encode::Term>> checks;
};

// Walks entry as walk() does, but each time the walk reaches the loop headed
// by header, walks one iteration of it as a Transition instead of unrolling
// it, appends that to out, and goes on after the loop from the iteration's
// exits, which over-approximate every way out of the loop. A pointer phi at
// the header, or a write the pre-state cannot havoc, is unsupported.
llvm::Error transitions(llvm::Function &entry, const llvm::BasicBlock &header,
                        const Options &opts, encode::Encoder &enc, memory::Memory &mem,
                        checks::Checks &checks, Trace &trace, std::vector<Transition> &out);

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
