# Endless loops that do nothing end the path

Recorded 2026-11-24.

`do { } while (1); assert(0);` (`tests/esbmc/github_2602_1.c`) made the
walker unroll forever without `--unwind`, printing one "Unwinding loop" line
per iteration, and fail a pointless unwinding assertion with it. ESBMC turns
such a loop into `assume(0)`, as CBMC's self-loops-to-assumptions does. LLVM
has no pass for it: loop-deletion deletes only loops it can prove finite, and
deleting this one would make the code after it reachable, which is wrong.

## The rule

The walker's `loop()` ends the paths that reach a loop's header, as at
`unreachable`, when `endless()` holds for the loop: every edge out of it is a
branch on a constant that never takes it, and no instruction in it has a side
effect (`mayHaveSideEffects()`, which covers stores, volatile and atomic
accesses, fences and the trapping intrinsics) or calls anything but an
intrinsic without one. No path leaves such a loop and nothing it does can be
seen, so ending the path there loses nothing. A check is a call
(`__VERIFIER_assert`, `reach_error`) or an edge out of the loop to an
assert's, a sanitizer trap's or a `__builtin_unreachable`'s block, so a loop
with a check never qualifies.

Two choices:

- The edge out is ignored only when it is a branch on a constant. Clang keeps
  `br i1 true, %do.body, %do.end` for `do { } while (1)`, with `do.end` an
  `unreachable` that InstCombine left, so `hasNoExitBlocks()` alone misses
  the case that motivated this. Ignoring every exit whose block can only
  reach `unreachable` instead would be unsound: a bare `unreachable` is
  `__builtin_unreachable`, which the walker checks, so
  `while (1) if (x) __builtin_unreachable();` would lose its failure
  (`tests/vereda/endless_loop_unreachable_false.c`).
- The rule lives in the walker, not as an `ir` transform that replaces the
  loop's entry with `unreachable`. The walker would check that `unreachable`
  as `__builtin_unreachable` unless a noreturn call came before it, deleting
  the loop would renumber the loops after it (ESBMC's "loop N" in property
  ids and unwinding messages), and the IR, its debug locations and so the
  trace and witnesses stay as clang wrote them.

LoopInfo never nests a loop with no exit inside another, since its blocks
cannot reach the outer header, so an endless loop written inside a loop that
ends is a sibling there and is reached as its own top-level loop. An endless
loop around a loop that ends is endless as a whole: the inner loop's
unwinding assertion is not checked either, since nothing after either loop is
reached (`tests/vereda/endless_loop_unwind.c`).

When a termination property is added, this rule must not apply under it.

## Imported tests

Of the 306 imported tests whose sources have a `while (1)`, `for (;;)`,
`for (...;;...)`, `do { } while`, `L: goto L` or a `while (x);`, the rule
fires in two: `esbmc/github_2602_1` (KNOWNBUG; it ran out of output space or
time before, now SUCCESSFUL as expected; promoted to CORE) and
`esbmc/31_cdaudio` (its `ERROR: goto ERROR;`, SUCCESSFUL before and after).
Most of the rest need a flag Vereda lacks and do not run. No wrong verdict.

## SV-COMP

All 1,407 tasks of the 2026-11-23 ReachSafety run, as that run does them
(`--incremental-bmc`, unreach-call, 60 s) but on a loaded machine, with a
binary that printed a line whenever the rule fired; the 644 that timed out
ran again at `--unwind 1`. Of the 27 that still timed out, four have a
`while (1)`, each with a call or a break in it, and the rest none. It fires in four tasks, each at a CIL-style exit:
`_EXIT: goto _EXIT;` in list-ext-properties/list-ext and
list-ext2-properties/simple_search_value-1 and -2, `STUCK: goto STUCK;` in
loop-invgen/string_concat-noarr. None changes verdict against the binary
without the rule: list-ext and simple_search_value-2 stay FAILED (expected),
simple_search_value-1 and string_concat-noarr stay TIMEOUT, the latter
reaching the same bound with one unwinding check fewer.
