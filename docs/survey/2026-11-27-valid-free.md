# Valid free checks (#30)

Recorded 2026-11-27.

## What landed

Each `memory::Memory` object now has an immutable kind: static (globals,
string literals, functions whose address is taken), stack (allocas, byval
copies, variadic areas) or heap (`malloc` and `calloc`, and so `realloc`,
`strdup` and the other models built on them). Liveness is path state:
`memory::State::dead` is an SMT array from object id to Bool, a constant
array of false in `Memory::initial()`. `Memory::free` stores true at the
object of the freed pointer, and `Memory::ite` joins the two sides' arrays,
so branch joins, return joins and the states the incremental walk saves at
a checkpoint all carry it with no extra code. An object is live where it
exists and is not dead; #29 (stack objects expire at return) and #31/#32
(leaks) read the same array.

`Memory::invalidFree(st, p)` gives the four conditions, worded and named as
ESBMC 8.8 reports them, and the walker turns each into a check at the call:

| condition | ESBMC message | id |
|---|---|---|
| object 0 at a nonzero offset (made from an integer) | `dereference failure: invalid pointer freed` | `invalid-pointer-freed` |
| a static or stack object | `dereference failure: free() of non-dynamic memory` | `free-non-dynamic-memory` |
| any object at a nonzero offset | `Operand of free must have zero pointer offset` | `free-non-zero-offset` |
| a heap object already dead | `dereference failure: invalidated dynamic object freed` | `invalidated-dynamic-object-freed` |

Null is a valid free. The objects of a symbolic pointer are those
`Memory::candidates` gives (a choice of constants, or every exposed object
for a pointer read from memory), so a symbolic alias, a pointer stored and
read back, and an address converted back to a pointer are all covered. The
dead condition reads the array once at the pointer's object rather than once
per candidate: a pointer into an object exists only where the object does,
so `exists` adds nothing. On a 10-node list freed node by node (`--unwind
12`, single runs on a shared machine) the solve took 92 s with a read per
candidate and 80 s with one; it takes 70 s with `--no-pointer-check`, the
difference being mostly the extra solver phase that a non-unwinding check
brings. Unchanged programs pay
nothing: without a free the array stays the constant one and every join
folds.

After the checks the path goes on only where the free was valid (the guard
is conjoined, as the alignment check does, never a global assumption), so
the first invalid free is reported and later frees of the same object are
not reported again on that path. A free whose checks do not count (under
`--property unreach-call`, `no-overflow` or `--no-pointer-check`) still kills
the object but prunes nothing.

`realloc`'s model now calls `__vereda_check_free(p)` before it allocates, so
an invalid `p` is reported even where the new allocation fails (C17
7.22.3.5p3), as ESBMC's `symex_realloc` does. A check reached inside a
library model, which has no debug info, is reported at the innermost call on
the stack that has a location (the walker's new `site`, saved and restored
like `reachError`), so the model's two checks of `p` are one claim at the
program's `realloc` call, with the ESBMC id `main.invalid-pointer-freed.1`
the imported `github_10301_fail` expects.

## Defaults and properties

As in ESBMC, the free checks are part of the pointer checks: on by default,
off with `--no-pointer-check`. `--property valid-free` counts only them and
prints `FALSE_FREE`, as ESBMC's SV-COMP wrapper does; it conflicts with
`--no-pointer-check`. The front end still rejects the valid-memsafety
property file, whose valid-deref and valid-memtrack checks do not exist yet:
accepting it would answer TRUE without them.

## Decisions not taken, and limits

- A host-side map of per-object dead flags (as `CellMap` holds bytes) would
  avoid array reads but merge a flag per differing object at each join. The
  array is simpler and its reads fold for constant objects; revisit if a
  free-heavy benchmark shows the array theory in the solve time.
- Allocation site and stack-frame ownership are not recorded yet: no check
  here needs them. #29 needs the frame and #31 the site.
- Freeing a pointer the program got from outside (a bodyless call's result,
  memory a bodyless call havocked) is an invalid pointer freed, as ESBMC has
  it; `github_9368_fail` now fails that way because `posix_memalign` has no
  model.
- Frees reached through Rust's allocator shims (`__rust_dealloc`,
  `__rust_realloc`) release without checks. Rust's standard library
  functions without a body (`RawVecInner::deallocate` in
  `tests/vereda/rust_heap_safe.rs`) havoc the memory that holds a `Box`'s
  pointer, which made the `Box`'s drop an invalid free.
- A freed object keeps its addresses: a later allocation never reuses them,
  so `github_9221_fail` and `memspace_freed_heap_reuse_fail` still pass where
  ESBMC fails.
- `fclose` has no model, so closing a stream twice is not found
  (`github_5565_fclose_double_free_fail`).

## Tests

New tests under `tests/vereda/free_*`: a failing test for each condition
(double, interior, non-dynamic over a local, a global and a literal,
integer), a conditional free across a join, a symbolic alias freed after a
callee freed one side, realloc of a freed pointer with a size that always
fails and realloc to zero size before a free, the property and flag wiring,
and a passing test of null, failed, zero-size, calloc, callee, loop and
realloc frees. The double free runs at 64 and 32 bits, under
`--incremental-bmc` and under `--incremental-solver`.

Of the 378 imported tests whose source calls `free` or `realloc`, 18 FUTURE
tests now pass and are CORE. Three more now fail as expected but with other
wording than ESBMC's (`deref_unmodelled_free` reports the global it frees as
non-dynamic, `deref_unmodelled_free_interior` the offset, and
`github_9368_fail` the missing `posix_memalign` model) and are re-marked.
`struct_bitfields_02` is now KNOWNBUG: its `ptr->c` reads bytes 4 to 7 of a
5-byte allocation, bytes past the object are nondet, so the guarded frees
can run twice; ESBMC reports success.

Follow-up issues: #354, #355, #356, #357, #358, #359.
