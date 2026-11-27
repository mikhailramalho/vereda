# One loop iteration as a transition relation

Recorded 2026-11-27.

## Background

Plan §4.5, choice 4 (#16): one pass through a loop's body, from a state at its
header to the state at its back edge, as a call of its own. k-induction (#34),
the recurrent-state check (#39), ranking functions (#40), invariant checks and
loop summaries all need it; none of them is part of this entry, which only
builds the relation and tests it.

The walker already has the pieces. `loop()` unrolls by putting an arrival on
the header, walking the loop's region once, and finding the back edge's
arrivals on the header again; `summarise()` (analysis/loops.h) walks one
iteration from header values it makes itself (those at a nondet index). The
relation is a third way to walk the same region: from header values and
memory that stand for any iteration.

## Interface

`walker.h` gains `struct Transition` and `walker::transitions()`:

```cpp
struct Transition {
  std::vector<encode::Term> pre;   // header phis, in order, as the iteration starts
  memory::State preMem;            // memory as the iteration starts
  encode::Term next;               // the iteration takes a back edge
  std::vector<encode::Term> post;  // header phis there
  memory::State postMem;           // memory there
  std::vector<std::pair<size_t, encode::Term>> checks; // raised in the iteration
};

llvm::Error transitions(llvm::Function &entry, const llvm::BasicBlock &header,
                        const Options &, encode::Encoder &, memory::Memory &,
                        checks::Checks &, Trace &, std::vector<Transition> &out);
```

`transitions()` walks `entry` as `walk()` does, except that each time the walk
reaches the loop headed by `header` it walks one iteration from the pre-state
below instead of unrolling, appends the iteration to `out`, and goes on after
the loop from the iteration's exits. The post-state is a function of the
pre-state's symbols (and of nondet values and the values the loop reads from
outside), so the relation is `next ∧ post = f(pre)` with no extra terms:
a client substitutes (`Terms::substitute`) or equates symbols to chain
iterations. Each entry of `checks` is a check's index in `checks` and what
violates it in this iteration; the check itself is shared with every other
place that reaches the same construct (one claim per construct), so its
`violated` is the disjunction over all of them.

Walking from the entry function, rather than from the loop alone, gives every
value the loop takes from outside (bounds, pointers to arrays, globals) the
term it has where the loop is entered, and keeps the facts (encoder.h) known
there: they are about terms the loop does not change.

The walk after the loop goes on from exits out of an arbitrary iteration,
which over-approximates every way the loop can end. That is what the step
case of k-induction needs after the loop; a client that wants plain BMC there
does not call `transitions()`. Checks raised in the iteration hold from an
arbitrary state, so solving them as BMC checks is not BMC: the client decides
what they mean (k-induction assumes them for the first k iterations).

A loop reached several times (inside an outer loop that is unrolled, or in a
function called more than once) gives one `Transition` per visit, each with
its own symbols. No user-visible flag is added and no dump: the unit test
reads the terms directly.

## Pre-state

- Header phis: a fresh value of the phi's type each (`Encoder::fresh`). A
  pointer phi, or one of a type holding a pointer, is unsupported for now: a
  fresh pointer points only to exposed objects (encoder.cpp), which would miss
  the arrays a `p++` loop walks. The sound pre-value is a fresh offset into one
  of the phi's underlying objects (`getUnderlyingObjects`), which is a small
  follow-up.
- Memory: the state merged from the arrivals at the header, with every object
  the loop may write given nondet contents (`Memory::havoc` of the whole
  object). Every other object keeps the contents it has as the loop is
  entered, which is where the relation is more precise than havocking all
  memory.
- Initialisation, under `--ub-uninit-check`: `havoc` marks the object it
  writes initialised, as an unknown call may write it all, but a loop may
  leave some bytes untouched. So each byte of a written object is
  initialised in the pre-state if it was before the loop or an earlier
  iteration may have written it (the old bit or a fresh one). Keeping only
  the old bit would report bytes an earlier iteration wrote; marking all of
  them initialised lost the reads of bytes the loop never writes. An object
  of unknown size or above `Memory::MaxBytes` is unsupported under the check.

`analysis::writtenObjects(l)` (new, `analysis/writes.h`) lists the objects:
for each store, atomic, memset/memcpy/memmove destination and each pointer a
bodyless call may write through, in `l` and in every function `l` calls
(transitively), the underlying objects of the pointer. Each must be a global,
or an alloca of `l`'s function from before `l`; an alloca in `l` or in a
callee is new in each iteration or call and is left out. Anything else, a
pointer argument, a pointer loaded from memory, a heap object, an address made
from an integer, throws `vereda::Unsupported`, as does a call without a body
that may write memory other than through its arguments (any not modelled by
the walker as writing nothing: the checks, assumptions, nondet values, malloc,
calloc and free). The walker then requires each object to hold no pointer
(`holdsNoPointers`, the test the walker already uses for array partitions),
because `havoc` gives the bytes integer provenance: a pointer stored in a
written object before the loop would come back pointing only to exposed
objects. Keeping the provenance and havocking only the data would lift this;
it belongs in memory, which other work is changing now, so it is a follow-up.

Object granularity is coarse: a loop that writes `a[i]` havocs all of `a`,
including the elements it has not reached. Invariants (plan §6) are what
narrow that, and they are a client of this relation, not part of it.

## Nested loops and calls

An inner loop of the iterated loop is walked as the walker always walks it:
unrolled up to `--unwind` (its unwinding check is one of the iteration's
checks) or summarised. A summary whose SCEV refers to the iterated loop's
AddRec gives up, as the iteration number is unknown (`frame.iteration` has no
entry for the iterated loop), and the inner loop is unrolled instead. Taking
the transition of an inner loop works the same way from the outside: the
walker unrolls the outer loop and reaches the inner header once per outer
iteration.

A call in the body is walked in its own frame as usual; the write analysis
follows it, so its stores to globals are havocked in the pre-state. A
transition reached again inside its own iteration (through recursion) or
inside an array loop summary's iteration throws `Unsupported`.

## Decisions

Two choices were settled on review. The walk goes on after the target loop
from the havocked iteration's exits rather than stopping there, as the step
case of k-induction needs the rest of the program walked from them. And the
relation stays a library call: the driver gets no path to it and no debug
flag until k-induction (#34) uses it.

## Not in this entry

The exits' guard as a term of its own, a relation over memory as a formula
(the states are there, a client compares the objects it cares about),
pointer phis, heap objects and objects holding pointers, and anything that
uses the relation: k-induction, recurrent states, ranking functions.

Follow-up issues: drafted in the pull request description, not yet filed.
