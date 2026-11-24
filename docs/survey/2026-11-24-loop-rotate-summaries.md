# loop-rotate against the loop summaries: not adopted

Recorded 2026-11-24. Part of issue #87.

The question from #87: does LLVM's `loop-rotate` in the pass list let the
loop summaries of #124 (`analysis/loops.h`, survey
`2026-11-23-array-loop-summaries.md`) summarise more loops? It does the
opposite. Rotated, almost no loop has the shape the analysis wants, SCEV
counts no more loops than before, and rotation changes what `--unwind N`
means, renumbers loops, moves loop locations and puts assignments the
program never made into counterexamples. It stays out of the pass list.

## Setup

Two scratch builds of main at a0e43e9e, LLVM 21, neither committed. The
variant runs `loop-simplify,loop-rotate,lcssa` where main runs
`loop-simplify,lcssa`, in `ir/load.cpp` and in the pass list the models are
built with; LoopRotate's defaults (header duplication on, headers of up to
16 instructions). Both builds print, for every loop of every defined
function in the module, whether `analysis/loops.h` accepts it and, if not,
the first test it fails; whether SCEV has a backedge-taken count for it;
whether it is in rotated form (its latch exits); and, as the walker runs,
each loop it summarises and each accepted loop it declines.

- Suite: all 6,766 tests with each build, 8 at a time each, 60 s a test,
  `REGRESS_STATS` on. Main passes all of them.
- ReachSafety: the 1,407 tasks of the 2026-11-23 baseline, with the
  property file, walked once at `--unwind 1`, stopping after the walk.
  Every loop reachable within a first iteration of the loops around it is
  walked, so the summarised count is a lower bound on a full run's.

## Loops

Loops of the program's own functions. The library models add 1,277 loops
to the suite's modules and 62 to ReachSafety's; neither build accepts any
of them.

| | suite, main | suite, rotated | ReachSafety, main | ReachSafety, rotated |
|---|---|---|---|---|
| loops | 2,011 | 2,011 | 3,235 | 3,235 |
| innermost | 1,675 | 1,675 | 2,874 | 2,874 |
| in rotated form | 50 | 1,907 | 2 | 2,938 |
| accepted by analysis/loops.h | 350 | 35 | 585 | 97 |
| summarised by the walker | 111 | 13 | 336 | 72 |
| tests or tasks with a summary | 95 | 12 | 192 | 46 |
| innermost, SCEV has a backedge-taken count | 942 | 949 | 1,072 | 1,069 |

The analysis wants a loop that leaves at its header, after the header's
exit count, with no store or call in the header (the header runs once more
than the body). Rotation moves the exit test to the latch. Of the
innermost loops the rotated build rejects, 1,139 in the suite and 1,925 in
ReachSafety fail "the header exits the loop", and 31 and 373 have a store
or call in a header that is now the whole body. The 35 and 97 loops it
still accepts are all single-block loops, header and latch in one, with no
store and no call: loops that only count. Not one loop that writes `a[i]`,
the shape #124 is for, is summarised once rotated.

Nor does SCEV count more loops in rotated form: it has a backedge-taken
count for about the same innermost loops either way (942 against 949, 1,072
against 1,069). So teaching the analysis to accept a latch exit would gain
nothing from rotation itself. Such a rule would help the loops that are in
rotated form in the source already, do-while loops and the like (50 in the
suite, 2 in ReachSafety), without rotating anything.

### The one gain, and its real cause

Ten ReachSafety tasks summarise more loops rotated: `loops-crafted-1/in-de*`
(in-de62: 1 to 6) and `loop-invgen/seq-3`. Their loops run one after the
other, each starting from what the one before left, as in

```c
while (x > 0) { x--; y++; }
z = y;
while (z > 0) { x++; z--; }
```

The analysis accepts both loops in both builds, but on main the walker
declines the second: its header phis start from the LCSSA phis of the
first loop, which SCEV sees through to an AddRec of the first loop, and the
walker's `evaluate` has no term for an AddRec of another loop. Rotated, the
second loop's start values come through a phi that also takes the guard's
skip edge, which SCEV cannot see through, so the walker uses the term it
bound for it. On main the walker declines 79 accepted ReachSafety loops in
59 tasks this way (array-crafted, loops-crafted-1, loop-invariants,
array-examples, ...). Evaluating such an AddRec at the earlier loop's exit
count, or using the term already bound for the LCSSA phi, would win these
loops without rotation; that is a change to the walker, left for #36.

## Verdicts and time in the suite

No wrong verdict. Eighteen tests fail, three of them out of memory, and
twelve more time out at 60 s, which run.py counts as skipped:

- 15 summary tests, no longer summarised, unroll their 100,000 iterations
  and time out or run out of memory: ten esbmc/array_loop_summary tests
  (the plain one, `_counter`, `_decrement`, `_fail`, `_nondet`,
  `_nondet_fail`, `_store`, `_store_fail`, `_stride`, `_stride_fail`) and
  `vereda/summary_affine`, `summary_affine_fail`, `summary_checks`,
  `summary_checks_fail` and `summary_update_fail`. Four
  more (`summary_check_fail`, `summary_conditional`, `_conditional_fail`,
  `summary_nondet_macro_fail`) keep their verdict but no longer print
  `Summarised 1 array loop(s)`.
- `--unwind N` changes meaning. Unrolled, a rotated loop of N iterations
  needs bound N, where main needs N + 1, as ESBMC does: on
  `vereda/incremental_bmc` (a loop of 4 iterations) ESBMC fails the
  unwinding assertion at `--unwind 4` and passes at 5. `incremental_bmc`,
  `loop_const`, `summary_off`, `dump_dir_incremental`,
  `dump_smt_too_incremental` and `multi_property_incremental` fail on that.
  The same shift lets one more iteration through at a given bound:
  `esbmc/github_1067` (`--unwind 1`) goes from FAILED to UNKNOWN, as the
  path now reaches a `realloc` copy of symbolic size, and three FUTURE
  tests (`github_4978`, `github_4979`,
  `vasprintf_const_format_no_overflow`) go from UNKNOWN to the right
  SUCCESSFUL, done at bound 13 before the walk at bound 14 reaches an
  `llvm.ptrmask` Vereda does not support.
- `esbmc-k-induction/github_5188_bounded` goes from FAILED to SUCCESSFUL,
  ESBMC's answer, because the bug past the bound was found only by the
  summary.

`tests/bench.py --list`, side by side, on the 65 tests whose summarised
loops changed with the same verdict in both suite runs, plus
`13_ludcmp_new`:

| test | wall s | peak MB |
|---|---|---|
| esbmc/array_loop_summary_decrement_fail | 0.15 → 21.08 | 35 → 1066 |
| esbmc/array_loop_summary_counter_fail | 0.14 → 15.61 | 36 → 625 |
| vereda/summary_check_fail | 0.14 → 12.22 | 36 → 441 |
| esbmc/array_loop_summary_vla | 0.14 → 2.08 | 36 → 76 |
| vereda/mem_words_fail | 0.13 → 1.65 | 36 → 128 |
| esbmc/array_loop_summary_vla_fail | 0.14 → 1.19 | 36 → 76 |
| vereda/summary_conditional | 0.14 → 0.96 | 36 → 52 |
| esbmc/13_ludcmp_new | 64.76 → 47.90 | 336 → 318 |

The first three are regressions by bench.py's margins; the other tests
change by less than 0.5 s. `13_ludcmp_new` gets faster: it runs with no
`--unwind`, one solver call per iteration, and the rotated build walks 117
iterations where main walks 172, since each loop's last exit test is no
longer an iteration of its own. Over the 6,356 tests with the same verdict
in both runs the wall time adds up to 1,270 s on main and 1,322 s rotated;
the whole ctest run took 301 s and 442 s, the difference mostly the tests
that now time out.

## Traces and witnesses

- Witnesses: 16 FALSE ReachSafety tasks with loops, run as the baseline
  runs them (`--incremental-bmc`, property file) with `--witness`: the same
  verdicts. Eleven witnesses are identical; five (sum01_bug02, string-2,
  insertion_sort-1-2, invert_string-1, for_bounded_loop1) hold other values
  or one waypoint more or less, as the rotated build finds another path at
  a given bound. CPAchecker 4.3
  confirms both builds' witnesses for sum01_bug02, string-2,
  for_bounded_loop1, trex01-1 and array-2, and answers UNKNOWN for both on
  invert_string-1, insertion_sort-1-2 and vogal-2. Vereda's witnesses have
  only `function_return` waypoints and the target today (#14 item 4), so
  this checks little of what rotation moves.
- `&&` and `||`: clang lowers a short-circuit loop condition so that the
  loop leaves from the block where the operands join, not from the header.
  LoopRotate only rotates loops whose header exits, so these loops are left
  as they are and the branches of their operands keep their own locations.
  A loop with a single condition gets its test twice, as a guard before the
  loop and in the latch, both at the condition's location; each time the
  source evaluates the condition maps to one of them (the guard first, the
  latch after), so branching waypoints could still be placed.
- Counterexamples: LoopRotate folds the latch into the block above it and
  hoists its cheap instructions above the exit test, debug records and all.
  In `while (__VERIFIER_nondet_int()) n++;` the counterexample shows
  `n = 5` on the pass that leaves the loop and `n = 4` at the exit: an
  assignment the program never made.
- Loop ids and locations: rotation changes the order of loop headers in the
  reverse post-order that numbers loops, so `loop N` (`main.unwind.N`, the
  unwinding assertion's text) names another loop: the same loops change
  numbers in 35 functions of the suite and 53 of ReachSafety. The location
  of 17 suite loops and 66 ReachSafety loops moves by a few lines, mostly
  loops built from `goto` (the reducer-made tasks) and the longjmp tests.

## Decision

Not adopted. To be worth it, rotation would need a latch-exit rule in
`analysis/loops.h`, the walker's iteration count shifted back to ESBMC's
`--unwind` meaning, loop numbers taken from source order, and the debug
records of hoisted instructions dropped; and after all that SCEV would
count no more loops than today. #87 noted that `loop-idiom` converts
nothing without rotation; these costs fall on that pairing too. Two
follow-ups come out of the measurement, neither needing rotation: the
walker's declined loops whose start values come from an earlier loop
(79 ReachSafety loops in 59 tasks), and a latch-exit rule for do-while
loops.
