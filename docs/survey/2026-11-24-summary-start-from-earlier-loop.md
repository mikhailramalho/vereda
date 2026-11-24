# Loop summaries that start where an earlier loop stopped

Recorded 2026-11-24. Part of issue #36.

The loop-rotate survey (#162, `2026-11-24-loop-rotate-summaries.md`) found
loops that `analysis/loops.h` accepts but the walker unrolls because their
start values come from a loop before them, as in `loops-crafted-1/in-de*`:

```c
while (x > 0) { x--; y++; }
z = y;
while (z > 0) { x++; z--; }
```

The second loop's header phis start from the LCSSA phis of the first. SCEV
sees through an LCSSA phi, so it writes the start as an AddRec of the first
loop, `{n,+,-1}<loop 0>`, and the walker's `evaluate` had no term for an
AddRec of another loop.

## Design

When the walker reaches a loop, every loop before it has been walked,
unrolled or summarised, and every value the loop reads from outside it has
a term. In LCSSA form a value leaves a loop only through a phi in an exit
block, and the term of that phi is what the value holds on the way out,
merged over the exits taken when the loop was unrolled. So `summarise`
first collects the loop's inputs, the instructions outside it that its
instructions use, and maps the SCEV of each to its term. `evaluate` looks a
SCEV up in that map before anything else; an AddRec of an earlier loop, or
an expression of one, gets the term of the input SCEV gives it. No exit
value is computed from the earlier loop's trip count: if no input has
exactly that SCEV, `evaluate` still gives no term and the loop is unrolled.
`getSCEVAtScope` would compute the exit value from SCEV's count of the
earlier loop, which may rest on no-wrap flags the walker does not assume
when it unrolls that loop, instead of taking the value the walker left.

An input counts only when its term is current: it is outside the loop and
in no loop the summarised one is not in. A value of an enclosing loop is
bound to its value in the iteration being walked; a value of a loop already
left is bound to the last iteration unrolled, or to the summarised
iteration's index.

That check also fixes a wrong verdict on main. SCEV sees through an LCSSA
phi to a plain value inside the earlier loop as well, such as a nondet call
(`while (1) { x = nondet(); if (x < 30) break; }`), and `evaluate` used
that call's own term, the one of the last iteration unrolled, instead of
the one the path left with. `tests/vereda/summary_start_refused.c` gives
FAILED on main for a program that cannot fail. Now such a value comes from
the LCSSA phi when it is an input; when only an expression of it is (SCEV
counts the iterations of `for (i = 0; i != 3 * x; i += 3)` as `x`, and the
loop reads only `3 * x`), the loop is unrolled.

Still unrolled: an earlier loop's AddRec that SCEV folds into a new one no
input holds. A store loop that continues an earlier loop's index,
`for (; i < N; i++) a[i] = 2;` after `for (i = 0; i < n; i++) a[i] = 1;`,
writes from `{@a,+,4}<loop 0>`, the earlier store's address at its exit;
the input is `i`, `{0,+,1}<loop 0>`. Relating the two (the same loop and
proportional steps) would compute a value from the AddRec, which this
change avoids. Neither ReachSafety nor the suite has such a loop among the
ones declined.

## Numbers

c31cac6d (main) against the same plus this change, both built with a
counter that logs each loop summarised and each accepted loop declined,
with the reason `evaluate` gave no term. As in #162, ReachSafety's 1,407
tasks of the 2026-11-23 baseline run with the property file at
`--unwind 1`, stopping after the walk, so the counts are lower bounds; the
suite's 6,791 tests run with their own flags, also stopping after the walk.

| | ReachSafety, main | ReachSafety, this | suite, main | suite, this |
|---|---|---|---|---|
| loops summarised | 400 | 434 | 112 | 112 |
| tasks or tests with a summary | 209 | 210 | 96 | 96 |
| accepted loops declined | 171 | 137 | 210 | 210 |
| for an AddRec of an earlier loop | 37 | 0 | 0 | 0 |
| for an AddRec of an enclosing loop | 20 | 19 | 12 | 12 |
| header phi not an AddRec (reductions) | 51 | 51 | 1 | 1 |
| constant count of at most 256, no store | 62 | 66 | 195 | 194 |
| other | 1 | 1 | 2 | 3 |

(The suite's counts are of the tests on main, without the five added
here; with them it summarises 118 loops in 100 tests.)

Of the 37 ReachSafety loops, 34 are summarised now and 3 (in
`array-examples/standard_find_ground-1`, `standard_strcpy_ground-1` and
`standard_vararg_ground`) get a count, a constant of at most 256, and are
unrolled by that rule. A fourth loop, in `loop-lit/cggmp2005b`, takes a
count from an enclosing loop's value now and is unrolled by the same rule.
The 34 are in 13 tasks: `loops-crafted-1/in-de*` (10 tasks, 1 to 5 loops
each), `loop-invgen/seq-3`, `down` and `up`. #162 counted 79 such loops in
59 tasks; it counted every accepted loop declined for a missing term at an
earlier main, most of them reductions such as `max = a[i] > max ? a[i] :
max`, whose header phi SCEV cannot write as an AddRec, a shape for the next
step of #36.

In the suite no summary changes. One refusal does: in
`vereda/witness_branching` the count of `for (j = 0; j < i - 2; j++)`
comes from a phi of a loop before it; main evaluated it to a small
constant from that phi's stale term and unrolled the loop, this change
declines it and unrolls the loop too.

## Verdicts and time

The 17 ReachSafety tasks whose counts changed, run as the baseline runs
them (60 s, `--incremental-bmc`, property file), with both binaries:

| tasks | main | this |
|---|---|---|
| `loops-crafted-1/in-de20` ... `in-de62` (10), `loop-invgen/seq-3`, `down`, `up` | timeout | TRUE, 0.15 to 0.20 s |
| `loop-lit/cggmp2005b` | TRUE, 0.43 s | TRUE, 0.39 s |
| `loops-crafted-1/vnew1` | timeout | timeout |
| `array-examples/standard_find_ground-1`, `standard_strcpy_ground-1`, `standard_vararg_ground` | timeout | timeout |

All 13 new answers are the expected TRUE; no verdict is wrong.
`vnew1` gains a summary but times out on its other loops.

`tests/bench.py --list`, side by side, on the five new tests,
`witness_branching` and five summary tests whose summaries do not change:

| test | wall s | verdict |
|---|---|---|
| vereda/summary_start_summarised | timeout → 0.13 | → SUCCESSFUL |
| vereda/summary_start_summarised_fail | timeout → 0.13 | → FAILED |
| vereda/summary_start_unrolled | 0.13 → 0.12 | SUCCESSFUL |
| vereda/summary_start_unrolled_fail | 0.13 → 0.13 | SUCCESSFUL → FAILED |
| vereda/summary_start_refused | 0.14 → 0.25 | FAILED → SUCCESSFUL |
| vereda/witness_branching | 0.14 → 0.14 | FAILED |
| vereda/summary_affine, summary_checks, esbmc/array_loop_summary, _counter, _store | 0.13 to 0.19, unchanged | SUCCESSFUL |

bench.py flags the two verdict changes as regressions; both are main's
answers that were wrong. `summary_start_unrolled_fail` passes
`--unwind 3 --no-unwinding-assertions`, and main, unrolling the second
loop, cuts the path that fails after 99 iterations; `summary_start_refused`
is the stale term above, unrolled now (0.1 s more).
