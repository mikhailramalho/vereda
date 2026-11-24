# Irreducible control flow through FixIrreducible

Recorded 2026-11-24.

How #176 handles irreducible control flow, which gave all five UNKNOWNs of
the ReachSafety rerun on a453547a, and what it gives on those tasks and on
the suite.

## Choice

LLVM's `fix-irreducible` pass (FixIrreducible) turns each irreducible
cycle into a natural loop without copying code: every way into the cycle,
and every edge back to the block CycleInfo takes as its header, goes
through a new block `irr.guard` whose branches pick the entry taken, on
phis set per incoming edge. `fixIrreducible` in `src/ir/load.cpp` runs it
on the functions CycleInfo finds an irreducible cycle in, after
InstCombine and before loop-simplify and lcssa. The walker's own check
stays, for what the pass leaves.

- The pass takes only branches, returns and unreachable. Such a function
  first goes through `lowerswitch`, which a longjmp restart switch and
  ESBMC's `github_7565_switch_entry` (a switch into a loop) need; a
  function with an indirectbr (computed goto) is left alone and the walker
  still gives UNKNOWN. Functions without irreducible cycles are not
  touched, so their switches stay.
- It runs after InstCombine. Run before, InstCombine set the guard phi on
  the edges from blocks it found dead to poison (two such edges in
  simple_array_index_value_4.i.v+lhb-reducer, under an if whose condition
  folds to false). That is harmless when the walker finds the same block
  dead, but InstCombine also counts a block reached only by undefined
  behaviour as dead, which the walker may still walk, and there the
  dispatch would go to any entry. After InstCombine the guards are the
  constants and conditions the pass builds.
- The guards' branches carry no location, so `ir::branchSite` gives them
  no branching waypoint and they make no trace step. A branch the pass
  redirects keeps its location and the name of its other successor, so a
  waypoint for that side stays right. `irreducible_false` checks the
  witness of a goto into a while loop: the if's waypoint, the while's
  three, nothing else.
- The new loop's location: unless its latches already agree on loop
  metadata (a while the cycle took in whole), it gets metadata with the
  location of one jump back to `irr.guard`; loop-simplify gives its
  preheader that location too. Without it the unwinding assertion of a
  loop made of gotos said line 0, as none of the new branches has one.

## Unwinding

The walker counts passes through `irr.guard`. On the tests here this is
ESBMC 8.3's bound: `irreducible_unwind` (6 for both), a goto into a while
(11 for both), `longjmp_into_loop` (3 for both), and the five tasks report
an unwinding failure at the same bounds 2, 3 and 5. It can be one more
than ESBMC's when CycleInfo takes another entry than the backward goto's
target as the cycle's header: a path that enters before that header then
passes `irr.guard` once on the way to it. `github_7565_switch_entry`
(a switch into three labels of a goto loop, 99 jumps back) needs
`--unwind 101` where ESBMC needs 100. Which entry is the header depends on
the order of branch successors, which InstCombine changes when it inverts
a condition; making the backward goto's target the header would need a
pass of our own, not done.

Loop summaries (#124) reject these loops: their head is `irr.guard`, which
never leaves the loop. `array_loop_summary_rejected` (a goto into a
summarisable loop) now gives the right verdict, and a goto into a
1000-element store loop is unrolled (9 s) to the right verdicts.

## Results

The five tasks, 60 s, `--incremental-bmc --property-file`, ILP32, clang
21.1.2, all expected TRUE. Before: UNKNOWN (irreducible control flow) in
0.2 s. The bound reached is the last `Bound k too small`; their reducible
variants on main 8f2b8df1 are next to them; the bounds vary by a few
between runs.

| task | now | bound | reducible variant (main) |
|------|-----|-------|---------------------------|
| simple_array_index_value_4.i.v+lhb-reducer | TIMEOUT | 15 | nlh-reducer: TIMEOUT, 17 |
| id_build.i.p+sep-reducer | TIMEOUT | 373 | i.v+lhb-reducer: TIMEOUT, 373 |
| id_build.i.p+nlh-reducer | TIMEOUT | 377 | |
| mod3.c.v+lhb-reducer | TIMEOUT | 3 | cfa-reducer: TIMEOUT, 15 |
| mod3.c.v+sep-reducer | TIMEOUT | 3 | |

ESBMC times out on all of them. Since #169's rerun, `--incremental-bmc`
raises the bound until a verdict or the time limit, so a TRUE task with an
unbounded loop times out instead of stopping at bound 50. The mod3
reducer tasks split the one loop into three nested ones (the outer one
and two FixIrreducible loops), so a bound k unrolls k³ bodies around
`x % 3` on a nondet `x`; at `--unwind 2 --no-unwinding-assertions`
Vereda answers in 7 s, where ESBMC times out after 120 s.

Suite tests that gave the irreducible UNKNOWN on main, found by running
every test with `--unwind 1`, and those whose IR now has an `irr.guard`:

| test | now |
|------|-----|
| esbmc/loop_simplify_goto_into_body | FAILED, right: FUTURE to CORE |
| esbmc/array_loop_summary_rejected(_fail) | SUCCESSFUL / FAILED, right; FUTURE for ESBMC's summary message |
| vereda/longjmp_into_loop | SUCCESSFUL (expected UNKNOWN before) |
| esbmc-k-induction/github_7565_switch_entry(_fail) | need `--k-induction`; under `--unwind 101` SUCCESSFUL / FAILED, as ESBMC |
| esbmc-k-induction(-parallel)/pc_sfifo_2_BUG | need `--k-induction`; `--unwind 10` FAILED at line 346, as ESBMC |
| esbmc-k-induction/pointer_write_heap_root_callee_cond_entry_fail | needs `--k-induction`; FAILED at line 35, as ESBMC |
| esbmc-goto2lean/unsupported_loop_leaves_branch | needs `--goto2lean`; SUCCESSFUL with `--no-unwinding-assertions`, as ESBMC |
| esbmc-unix/04_bzip2smp_01 | FAILED in 1.9 s, as on main (1.7 s): the walk never reaches the irreducible function |
| esbmc-unix/03_bzip2smp_comb_01(_new), 04_bzip2smp_03 | need flags Vereda lacks; past 100 s with `--unwind 1`, as on main |

Under ctest, the 500 tests that contain a goto or a setjmp and end within
60 s pass or skip as marked (85 and 415). IR preparation of
`ntdrivers/floppy2.i.cil.c` (732 KB, no irreducible cycle) takes the same
0.15 s as before.
