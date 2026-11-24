# Skipping the bounds a loop of known trip count still needs

Recorded 2026-11-24. Issue #175.

## Where the time went

Under `--incremental-bmc` every bound walks the program again from the
start. Four tasks ESBMC answers were still climbing at 60 s in the
ReachSafety rerun on a453547a (`2026-11-24-reachsafety-rerun-on-a453547a.md`).
Their outputs from that run:

| task | bounds walked | last bound | walk, total | solve, total | last walk |
|---|---|---|---|---|---|
| array-examples/standard_vector_difference_ground | 914 | 1827 | 50.4 s | 3.7 s | 0.119 s |
| array-lopstr16/partial_lesser_bound-1 | 1517 | 3033 | 50.6 s | 5.8 s | 0.065 s |
| loop-acceleration/array_2-1 | 921 | 1841 | 51.2 s | 3.2 s | 0.117 s |
| loop-acceleration/array_2-2 | 925 | 1849 | 50.9 s | 3.1 s | 0.116 s |

Each walk ends at the first loop, the only check is its unwinding
assertion, and its guard has folded to true: every path is still in that
loop, whose condition, `i < 2048` or `i < 100000` with `i` a constant in
each iteration, folds in every iteration. So the solve takes 3 to 7 ms and
the walk grows with the bound, which makes the total quadratic in it. The
loops are not summarised: in array_2-* and standard_vector_difference_ground
the first loop stores two nondet values, and a summary takes one store; in
partial_lesser_bound-1 every loop counts with a global `i` that it loads
and stores. The first three need bounds 2049, 2049 and 100001, which a full
unroll (no `--unwind`) reaches in 0.4, 0.3 and 12.6 s (8.3 s of walk, 1.2 GB).
partial_lesser_bound-1 needs 10,000,001, out of reach for any schedule; it
needs a summary that sees through the global counter.

The exit-teardown work (#185) measured that tearing down the solver and
term builder between bounds costs 12 to 14% of these runs, up to 18 ms a
bound; it shrinks with the number of bounds walked.

Across the 642 tasks the rerun did not answer, an 8 s run of a build that
printed each unwinding assertion's guard and SCEV's backedge-taken count
showed: in 94 the last walk ends in a loop every path runs past the bound
and whose count SCEV knows (1,000 to 100,000 iterations in most Arrays
tasks, 10^6 or more in `loops-crafted-1`), in 34 the guard is true but SCEV
has no count (counters in globals, `while (1)`), and in the rest the guard
is symbolic.

## The change

When the walk at bound k reaches a loop whose iteration k it reaches with a
guard that folds to true, and SCEV gives the loop a constant backedge-taken
count n, the unwinding assertion records n + 1, the bound that unwinds it
(`checks::Check::unwindsAt`). The driver skips the bounds of the
`--k-step` sequence below that, but never more than up to twice the bound
last walked, and never the largest bound under `--max-k-step`.

Why it is sound. A guard that folds to true means every path reaches the
loop's iteration k: a path that left earlier, ended at a bug, abort or
`exit`, or was cut at another loop's bound would make it a disjunction that
does not fold. The walk before the loop is then the same at every bound
from k on, nothing after the loop is reached below n + 1, and a bound in
between can only fail the same unwinding assertion or find a bug in the
iterations of the loop it adds. A bug any bound finds, every larger bound
finds too: the walker cuts a path only where it passes the bound, a summary
does not depend on the bound, and the solver checks assertions before
unwinding assertions. So a skipped bound can only delay a bug to the next
bound walked, never hide it, and a TRUE still needs every unwinding
assertion to hold. SCEV's count is only a hint: if it is wrong the walk at
n + 1 fails the assertion again and the schedule goes on from there.

Why the doubling. Under unreach-call a reached `reach_error` ends its path,
so a loop where a bug is reachable stops having a true guard. A failed
`__VERIFIER_assert` under `--property all` (the default) does not end its
path, so a loop of a million iterations with a bug in its third keeps a
true guard; jumping straight to bound 1,000,001 would walk all of them
before the solver sees the bug, where doubling finds it at bound 4
(`tests/vereda/incremental_bmc_trip_count_false.c`). The cost is about one
more walk of the loop in halves: 1, 3, 7, ..., 65535, 100001 for
standard_vector_difference_ground.

One case can still change: a bug in such a loop that only a skipped bound
finds, followed after the loop by IR the walker refuses (`Unsupported`),
would now give UNKNOWN where it gave FALSE.

## The four tasks

60 s each, one at a time, on the loaded machine of the run below (`-m32`,
`--incremental-bmc --property-file unreach-call.prp`):

| task | before | this change | jump straight to n + 1 | step max(2, k/8) |
|---|---|---|---|---|
| array_2-1 (FALSE) | timeout | 0.7 s, 12 bounds | 0.4 s, 2 bounds | 1.7 s, 52 bounds |
| array_2-2 (TRUE) | timeout | 0.7 s, 12 bounds | 0.4 s, 2 bounds | 1.6 s, 52 bounds |
| standard_vector_difference_ground (TRUE) | timeout | 25 s, 17 bounds | 13.7 s, 2 bounds | timeout at bound 72,416 |
| partial_lesser_bound-1 (TRUE) | timeout | timeout | timeout | timeout |

With main at 5d351dc0 merged in (#182, and #185, which exits without
tearing down the last bound's solver), this change takes 0.46, 0.43 and
14.3 s on the first three.

Growing the step with the bound (option 2 of #175) helps every task whose
bound climbs, not only these, but checks bounds a bug may need exactly
(`2026-11-24-k-step-2.md`) and still walks about nine times the last
bound: it loses standard_vector_difference_ground. Keeping the walk
between bounds (option 3) would redesign the bound loop and was not tried.

## ReachSafety

The 1,407 unreach-call tasks of Arrays, ControlFlow, Heap and Loops, with
the harness of the rerun on a453547a (60 s and 6 GB per task in a systemd
scope, 12 at a time, load average 20 to 30 from other agents), on this
change applied to 8f2b8df1. Every run the harness called ERROR was an
`oom-kill` in the journal, so they count as MEMOUT here:

| Run | Correct TRUE | Correct FALSE | Wrong | UNKNOWN | TIMEOUT | MEMOUT | Score |
|---|---|---|---|---|---|---|---|
| a453547a (rerun) | 457 | 303 | 0 | 5 | 640 | 2 | 1217 |
| this change on 8f2b8df1 | 477 | 317 | 0 | 7 | 591 | 15 | 1271 |

That difference also holds the commits between a453547a and 8f2b8df1
(`loop-invgen/up`, `down` and `seq-3`, the `loops-crafted-1/in-de*` tasks,
`floppy.i.cil-3`, `kbfiltr.i.cil-2`). 159 tasks skipped at least one bound.
Those, and 7 tasks whose verdict changed without a skip, ran again with
8f2b8df1 and with this change side by side, 6 at a time each:

| Run | Correct TRUE | Correct FALSE | Wrong | UNKNOWN | TIMEOUT | MEMOUT | Score |
|---|---|---|---|---|---|---|---|
| 8f2b8df1 | 54 | 15 | 0 | 1 | 89 | 7 | 123 |
| this change | 65 | 29 | 0 | 1 | 55 | 16 | 159 |

The 25 tasks gained, all timeouts before: the nine `standard_copy*_ground`
tasks (FALSE in 28 to 31 s),
`standard_vector_difference_ground` (TRUE, 25 s),
`data_structures_set_multi_proc_trivial_ground` (TRUE, 16 s),
`array-industry-pattern/array_range_init` (FALSE, 11 s),
`array-programs/copysome1-1` and `copysome1-2` (50 and 56 s),
`array-crafted/bAnd3` and `bor3` (TRUE, 52 s; they timed out in the full
run, so they sit at the limit), `loop-acceleration/array_2-1` and `array_2-2`
(0.7 s), and in `loops-crafted-1` `Mono3_1` and `Mono4_1` (FALSE, 21 to
24 s) and `mono-crafted_1`, `_3`, `_7`, `_9` and `_14` (TRUE, 18 to 20 s),
whose loops run a million times. Nine timeouts became out-of-memory kills:
`loop-acceleration/nested_1-*`, `Mono5_1`, `Mono6_1` and `mono-crafted_8`,
`_10` and `_12`, which need 2^28 or 10^7 iterations, where the walk at bound
2,097,151 already takes 3.5 GB and the next doubles it, and two 2-D
`array-multidimensional` tasks, as seven others of that set already did on
8f2b8df1. No task lost its verdict and none got a wrong one.

On the 69 tasks both runs answer, the total time went from 302 s to 216 s.
Two are slower by more than 1 s: `nla-digbench-scaling/egcd3-ll_valuebound10`
(51 to 54 s), which skips no bound, and `memory-model/2SB` (22.7 to 23.7 s),
whose bug only bound 11 finds cheaply (`2026-11-24-k-step-2.md`): it now
walks bounds 1, 3, 7 and 11 instead of every odd one and spends the same
22 s solving at 11.
`ntdrivers/diskperf.i.cil-1` gives UNKNOWN (a memmove of symbolic size at
bound 29) with or without this change.
