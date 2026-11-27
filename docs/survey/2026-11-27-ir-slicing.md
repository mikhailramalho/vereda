# Slicing the rolled IR before the walk (#115), measured and not adopted

Recorded 2026-11-27.

## Question

The walker builds one hash-consed term graph and the solver receives only
what the checks reach, so the formula is already the checks' cone of
influence. The walker itself still computes every value. Does a static slice
of the IR before the walk, dropping what no check can depend on, make the
walk, the solving or the peak memory smaller?

## The slice built

A sound, conservative slice was built (`analysis::slice`, run by the driver
after `ir::load`, off with `--no-slice`, ESBMC's flag); the code is kept on
the branch `feat/ir-slicing-impl`, commit `2e6b4319`. It is a mark-and-sweep
over the whole module, as LLVM's ADCE, with memory added. It keeps

- every terminator, so every branch condition, loop exit and return value:
  the path guards, the unwinding assertions and a witness's branching
  waypoints stay as they were;
- every call with all its arguments, except a bodyless `__VERIFIER_nondet_`
  call whose value nothing kept uses (kept too under `--witness`, which
  records each such value);
- every instruction with a side effect, every volatile access, every alloca
  of variable size (the PTRDIFF_MAX check);
- every loop, and every function a loop calls, whole (see below);
- under `--ub-uninit-check`, every load (each is a check) and every store;
- every store into an object it does not follow.

It follows an object (an alloca, a writable global or a `noalias` call) when
its address flows only into the pointer operand of loads and stores,
getelementptrs, casts, phis, selects, comparisons with null and lifetime
markers. Every read of such an object is then a load whose underlying objects
(`getUnderlyingObjects` without a lookup limit) include it, and the walker's
`fromAddress` cannot land in it, since it lands only in exposed objects. A
store into followed objects is kept once a kept load may read one of them.
Constant globals are never followed, so a write into one, which the pointer
check reports, stays. The operands of every kept instruction are kept.

Why it is sound: a pure instruction makes no check (encode does not reach
checks), so every check comes from a call, a terminator, a store into a
constant object, a variable-size alloca or, under the uninit check, a load,
all of which are kept with their operands; the only other way a value
reaches them is through memory, where each kept load keeps every store into
what it may read; and since every branch stays, every check is reached under
the same guard. This holds while loads and stores carry no check of their
own; the dereference checks of #29 would make every access whose pointer may
be invalid a root.

Five ways the walker reads the IR's shape, not just its values, had to be
respected, each found by a test or by review and each with a regression
test on the branch:

- `endless()` ends the paths into a loop with no way out and no side effect,
  but checks the unwinding of one with a store (`vereda/endless_loop_store_false`);
- the array loop summaries (`analysis/loops.h`) take a loop that only counts
  or holds one store, and a summarised loop has no unwinding assertion: with
  the store of `for (i = 0; i < 1000; i++) g = i;` removed, `--unwind 5`
  went from FAILED to SUCCESSFUL. Whether a loop is summarised depends on
  every instruction in it and in what it calls, which is why loops and their
  callees stay whole;
- under `--ub-uninit-check`, a load whose value is only stored is checked at
  the copy's reads (`copyOnly()`); removing the dead store made the walker
  check the load itself and report padding copied into an unread global;
- a comparison with another address exposes an object to `fromAddress`;
- a user function named `__VERIFIER_nondet_...` has a body with effects.

A debug record whose value was removed is poison, which the walker encodes
as a fresh symbol and the counterexample showed with an arbitrary value, so
the branch makes `ir::dbgAssign` skip it. The counterexample then leaves out
the assignments the slice removes, as ESBMC's does for its slicer's.

## Measurements

Base `29759530` (main) against the branch, LLVM 23.1.6, on 10 cores shared
with other jobs; base and head ran side by side, three times, alternating
which started first; medians of three.

How much it removes: of the 1,407 unreach-call tasks of the local
sv-benchmarks checkout (as `--dump-ir-only` reports), 1,262 lose nothing,
108 lose one or two instructions, 18 lose 3 to 9 and 19 lose 10 or more;
904 instructions in all, at most 152 (`ntdrivers/diskperf.i.cil-1`) and 122
(`loop-invgen/apache-get-tag.i.p+sep-reducer`). The rest of these programs
feeds a branch, a call or a loop. The slice adds about 2 ms to IR
preparation on the largest (`ntdrivers/floppy.i.cil-3`, 0.049 → 0.051 s).

The 37 tasks that lose 3 or more, in SV-COMP's configuration
(`--incremental-bmc`, unreach-call, 60 s): no verdict changed; 12 time out
on both; over the 25 decided on both, walk 0.58 → 0.57 s, solve
41.62 → 41.48 s, wall 48.54 → 48.53 s in total, no task outside noise.

The 40 slowest regression tests of main's last Tests run, with
`tests/bench.py`, three runs: no verdict changed, nothing regressed or
improved. (`esbmc-z3/github_9222` solves 15% faster on the branch in
bench.py and 15% slower when run by hand, also with `--no-slice`: the
`dbgAssign` change leaves three fresh symbols out, and Z3's run varies with
them.) The issue's candidates, `12_lms_new` and `13_ludcmp_new`, already
walk in milliseconds on main. The whole suite passed on the branch, but two
FUTURE tests that stopped on unsupported IR in sliced code gave a verdict:
`esbmc/github_1426` ESBMC's FAILED, `esbmc/github_351_false` SUCCESSFUL
where ESBMC's FAILED comes from a dereference check Vereda lacks.

Before loops were kept whole the slice did more: `repro7521_w2048` walked in
0.06 s instead of 0.96 s and peaked at 64 MB instead of 506 MB, because the
loads of its 2048-field local sit in a loop; but that version changed
unwinding verdicts through the summaries, as above, and made `loops/heavy-1`
take three times the memory at the same bound, since summarising its inner
loop once the store was gone cost more than unrolling it (at `--unwind
200000`: 1.96 s and 325 MB summarised, 1.17 s and 139 MB unrolled with the
store, 0.37 s and 79 MB unrolled without it).

## Decision

Not adopted: the sound version gives no measurable gain on any benchmark we
have. The hash-consed terms already give the solver the cone of influence,
InstCombine already removes dead pure values, and once every branch, call
argument and loop stays, what is left to remove is a few stores outside
loops, which the walker runs once. A slice worth having would need to remove
loop bodies, which means deciding unwinding and summarisation on the sliced
loop on purpose (a summarised loop gets no unwinding assertion today,
since SCEV gives its trip count), and branches that guard nothing kept, which
changes the paths a witness describes. Both are larger designs than #115's,
to revisit when a benchmark shows walk time spent on values no check reads.

Follow-up ideas, not filed: summarising a loop with no store and no check
costs more than unrolling it (`loops/heavy-1` above); the walker shows a
debug record whose value is poison as an assignment of an arbitrary value.
