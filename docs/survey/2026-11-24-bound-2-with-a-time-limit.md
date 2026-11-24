# Bound 2 with a time limit under --property-file

Recorded 2026-11-24. Issue #174.

## The question

`vereda.py --property-file` passes `--k-step 2`, as ESBMC's SV-COMP
wrapper does (`2026-11-24-k-step-2.md`), so the bounds checked are 1, 3,
5, ... A bug that needs bound 2 is found at bound 3, and on some tasks
bound 3's solve is far slower:

| task | bound 2 | bound 3 |
|---|---|---|
| ldv-sets/test_mutex_unbounded-2 (FALSE) | bug, 6.3 s | over 60 s |
| list-ext2-properties/list_and_tree_cnstr-1 (FALSE) | bug, 1.2 s | bug, 40 s |
| list-ext2-properties/simple_and_skiplist_2lvl-1 (FALSE) | bug, 0.4 s | bug, 8.7 s |

(`memory-model/2SB`, the fourth task of that note, needs bound 10, which
takes 15 s; #194's skip already takes it from 1 to 3, 7 and 11, which
finds the bug in 24 s.)

The issue proposed checking every bound up to some small k, or for the
first seconds of the run, then stepping by 2.

## Which tasks can change

The 1,407 ReachSafety tasks and the runner of the #194 survey
(`2026-11-24-skip-bounds-of-known-trip-count.md`: 60 s and 6 GB per task,
`--incremental-bmc --property-file unreach-call.prp`), with head.tsv,
its full run of #194's head, as the reference. Checking more bounds
below some k changes nothing for a task answered at bound 1, nor for one
stuck at bound 1. A task that finished bound 3 without an answer has no
bug below 3, so bound 2 cannot answer it either; such a task can only
change through the bounds it checks past k, or through the skip of #194,
whose doubling counts from the bounds walked. So the candidates are the
812 tasks answered at bound 3 or more, unanswered at a bound from 2 to 33,
or that skipped a bound.

## Every bound, measured, and the schedules between, simulated

Main at ac2604b1 and the same binary with
`--k-step 1` ran side by side on those 812, 3 at a time each, with load
average 13 to 35 from other agents:

| run | Correct TRUE | Correct FALSE | TIMEOUT | MEMOUT | UNKNOWN | Score |
|---|---|---|---|---|---|---|
| step 2 (main) | 291 | 203 | 300 | 17 | 1 | 785 |
| step 1 | 288 | 199 | 307 | 17 | 1 | 775 |

Step 1 answers test_mutex_unbounded-2 and loses nine tasks of two kinds.
Four climb to hundreds of bounds and need every walk the step of 2
saves: `loop-acceleration/array_3-1` and `array_4`, `loop-lit/afnp2014`
(TRUE, 40 to 46 s) and `nla-digbench-scaling/cohencu-ll_unwindbound100`
(FALSE at bound 101, 42 s). The other five lose at bound 2 itself, whose
proof that there is no bug is far harder than bound 3's bug:

| task | bound 2 | bound 3 |
|---|---|---|
| nla-digbench-scaling/egcd2-ll_unwindbound5 (FALSE) | over 58 s | bug, 0.5 s |
| nla-digbench-scaling/lcm1_unwindbound10 (FALSE) | over 58 s | bug, 1.6 s |
| nla-digbench-scaling/prod4br-ll_unwindbound2 (FALSE) | over 58 s | bug, 0.3 s |
| nla-digbench-scaling/prodbin-ll_unwindbound2 (FALSE) | over 58 s | bug, 0.2 s |
| nla-digbench-scaling/freire1_valuebound5 (TRUE) | too small, 45.6 s | TRUE, 37.9 s |

(`egcd3-ll_valuebound10`, TRUE in 48 s under step 1 and a timeout under
step 2, sits at the limit: it took 51 to 54 s in the #194 runs.)

The walk and solve times each run prints per bound give the cost of every
bound up to where each run stopped, so the schedules between the two can
be replayed from them: a schedule's time is the run's own overhead plus
the bounds it checks, up to the first that answers, cut at 60 s. The 159
tasks that skipped a bound were left out of this, since their sequence
depends on the trip counts too; step 1 changed none of them.

| schedule (653 tasks) | gained | lost | time where both answer |
|---|---|---|---|
| step 1 up to 3, then 2 (bounds 1, 2, 3, 5, ...) | 1 | 5 | 1009.7 s -> 956.6 s (401 tasks) |
| step 1 up to 5, then 2 | 1 | 5 | 1009.7 s -> 1065.1 s |
| step 1 up to 11, then 2 | 1 | 5 | 1009.7 s -> 1130.4 s |
| step 1 up to 21, then 2 | 1 | 5 | 1009.7 s -> 1182.3 s |
| step 1 for the first second, then 2 | 1 | 5 | 1009.7 s -> 965.6 s |
| step 1 for the first 10 s, then 2 | 1 | 5 | 1009.7 s -> 1242.4 s |
| up to 3, bound 2 limited to 5 s | 0 | 0 | 1050.7 s -> 1036.2 s (406 tasks) |
| up to 3, bound 2 limited to 10 s | 1 | 0 | 1050.7 s -> 1047.7 s |
| up to 11, bounds 2 to 10 limited to 10 s | 1 | 0 | 1050.7 s -> 1214.3 s |

(The gains of egcd3-ll_valuebound10 the replay gives some rows are left
out, as above.) Every schedule that checks bound 2 without a limit loses
the five; a larger k only adds time, since the bugs that need an even
bound above 2 (2SB's 10) take longer than any limit that keeps the
losses away. A schedule by time is no better and depends on the machine's
load.

## The change

`--k-step-from N` raises the bound by 1 below N and by `--k-step` from N.
`--k-step-from-time-limit S` gives each bound this adds to those of
`--k-step` at most S seconds of solving, all its checks together; past
that, the run goes on to the next bound as if this one were too small.
That is sound for the reason #194's skip is: a bug a bound finds, every
larger bound finds too, and a TRUE still needs a bound whose checks all
hold. An added bound does not count as the last bound walked for #194's
doubling, so the bounds after it are those of `--k-step` alone. The
largest bound under `--max-k-step` is never limited.

`vereda.py --property-file` passes `--k-step 2 --k-step-from 3
--k-step-from-time-limit 10`: bounds 1, 2 (at most 10 s), 3, 5, ... It puts
them before the user's flags, since the binary takes the last of a
repeated flag, so a user's `--k-step` or limit wins. ESBMC has no flag
for either.

The cost is up to 10 s on every task that reaches bound 3, where bound 2
is hard.

## Measured

The 638 tasks where checking bound 2 can matter (answered at bound 3 or
more, stuck at bound 3, or skipping a bound, in head.tsv or in main's run
above) ran with main and with this change side by side, 3 at a time each.
The run was made with a first version that counted bound 2 as the last
bound walked, which turned the doubling of the 162 tasks that skip into
1, 2, 5, 11, ... and made five of them 4.6 to 9.6 s slower; those 162 ran
again with the final change and main side by side, and the table puts the
two runs together:

| run | Correct TRUE | Correct FALSE | TIMEOUT | MEMOUT | UNKNOWN | Score |
|---|---|---|---|---|---|---|
| main | 292 | 203 | 124 | 18 | 1 | 787 |
| this change | 292 | 204 | 124 | 17 | 1 | 788 |

test_mutex_unbounded-2 is answered (FALSE, 6.0 s) and no task is lost;
`array-lopstr16/break-2` went from an out-of-memory kill at 59 s to a
timeout. On the 495 tasks both answer the total went from 1678.9 s to
1695.4 s. list_and_tree_cnstr-1 went from 40.5 s to 1.3 s,
simple_and_skiplist_2lvl-1 from 8.6 s to 0.6 s, and four
`nla-digbench-scaling` tasks that bound 2 answers (freire1_valuebound1 and
2, knuth_valuebound20, lcm1_valuebound2) by 1.4 to 4.3 s each; 148 tasks in
all are answered at bound 2, most in 0.2 s either way. The
five bound-2 tasks of the table above pay the limit: the four FALSE ones
went from 0.2 to 1.6 s to 10.2 to 11.7 s and freire1_valuebound5 from 43
to 53 s. Bound 2 was passed over in 67 tasks, those five and 62 that time
out anyway.

All 1,407 tasks then ran with this change, 6 at a time, against head.tsv
(12 at a time under heavier load, on 8f2b8df1 with #194). A reinstall of
the system's LLVM 21 took its clang away for half a minute during the
run; the 450 tasks that found no clang ran again.

| run | Correct TRUE | Correct FALSE | Wrong | UNKNOWN | TIMEOUT | MEMOUT | Score |
|---|---|---|---|---|---|---|---|
| head.tsv | 477 | 317 | 0 | 7 | 591 | 15 | 1271 |
| this change on ac2604b1 | 481 | 318 | 0 | 2 | 589 | 17 | 1280 |

No task lost its verdict and none got a wrong one. Of the changes, only
test_mutex_unbounded-2 comes from this change. Five UNKNOWN tasks
(`*-reducer` variants in `loop-crafted`, `loop-industry-pattern` and
`loop-invgen`, irreducible before #190) now time out; `array-crafted/bAnd3`
and `bor3` (52 s), `nla-digbench-scaling/egcd3-ll_valuebound10` (51 s)
and `nla-digbench/ps5-ll` (58 s) are answered at the edge of the limit,
as in the #194 runs; `loops-crafted-1/Mono5_1` and `nested5-1` ran out of
memory before the time limit, as the #194 survey found Mono5_1 doing.
The times are not comparable with head.tsv's, which ran twice as many at
a time; the side-by-side runs above give the cost.
