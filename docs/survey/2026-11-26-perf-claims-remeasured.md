# Performance claims measured again on a quiet machine

Recorded 2026-11-26.

Earlier reviews timed several claims while the machine ran at a load near 95 on
80 cores, so their numbers could not be trusted. This entry measures them
again on a quiet machine: the one-minute load average, from all users, stayed
between 3 and 17 at the start and end of every batch but one, which ended at
34 and was repeated (item 9). It measures; it changes no code.

## Method

Each binary was built in its own worktree and build directory, Release, with
LLVM 23.1.6 from `~/tools/llvm-23` and camada 0.25. All builds and runs were
pinned to 35 cores (`taskset -c 35-69`), with at most 8 runs at a time, and
the runs of the binaries compared were interleaved, so that each repeat of
base and head ran side by side. Each run goes through `vereda.py` with the
test's own `// RUN:` flags, taken from that commit's tree, under the suite's
4096 MB address-space limit (6144 MB for SV-COMP tasks, as
`contrib/benchexec/vereda.xml`). Wall time includes clang. Peak memory is the
largest resident set of the run (vereda.py, clang and the binary, from
`wait4`); a run stopped by its time limit has no figure. Tables give the
median and, in brackets, the range over the repeats. Walk and solve are the
binary's own `Walk time` and `Solving time` lines, summed over bounds. The
scripts are a variant of `tests/bench.py` that repeats each pair.

| binary | commit | what |
|---|---|---|
| main | `93a561a5` | origin/main when this started (it already has #330) |
| noresume | main, with `Incremental::Impl` building its `Walk` with `incremental=false` | fresh-walk oracle for #287: every bound walks from scratch and solves with `decideNonIncremental` |
| nobl | main, with `boundedLimit = 0` under `--incremental-bmc` | #281 |
| rewalk | main, printing the time of each `freshWalk` call in `decideNonIncremental` | #247 |
| mb | `3ddfb3eb` | merge base of #330, #335 and #336 |
| pr330, pr335, pr336 | `33928d11`, `a0720829`, `ee0cba65` | the three branch heads (merged since, as #330, #335, #336) |
| p240b / p240h | `afc20c02` / `326fb695` | #240 and its parent |
| p238b / p238h | `31e0f08e` / `dc61ef2d` | #238 and its parent |
| p238nan | `31e0f08e` plus only the `Encoder::fcmp` hunk of #238 | splits #238 |
| p241b / p241h | `2850c0e0` / `70ad3e61` | #241 and its parent |
| p276b / p276h | `0862ec6e` / `aebbf301` | #276 and its parent |

## Summary

| # | claim | verdict |
|---|---|---|
| 1 | #287: a resumable bound under `--incremental-bmc` solves slower than a fresh walk | holds, but it is the solver mode and not resuming that costs, and it cuts both ways |
| 2 | a 16-bit table with one extra write never compresses | holds with `--no-summarize-array-loops`; out of memory from N = 5000 |
| 3 | #241 made github_302 solve slower, 16.9 → 26.1 s | holds (17.4 → 27.2 s, 557 → 756 MB); gone on main (6.6 s) |
| 4 | #276 table compression speeds solving up | holds: 15 to 120+ s down to 0.2 s on period 3 to 15 tables |
| 5 | #240 "Optimize..." | noise in time and memory |
| 6 | #238: 33 % on github_302 from dropping NaN checks | holds, and the NaN change alone accounts for it; the Terms rewrite gains 18 % walk time elsewhere |
| 7 | #247: the rewalk inside `decideNonIncremental` counts as solving time | holds, but it happens in 59 of 3,252 tests and costs 0.12 s in all |
| 8 | #281: unused `boundedLimit` work under `--incremental-bmc` | noise; not measurable |
| 9 | #330, #335, #336 | no regression; #330 is faster on picosat, not slower |

## 1. Resuming under `--incremental-bmc` (#287, #328)

A resumable bound keeps its solver, created incremental, and solves with
`decide()` (`checkSatAssuming`); a fresh bound solves with
`decideNonIncremental()`, which lets Bitwuzla run its non-incremental
preprocessing (`2026-11-25-non-incremental-solver.md`). So main against
noresume compares resuming plus incremental solving against fresh walks plus
non-incremental solving; noresume with `--incremental-solver` compares fresh
walks with the same incremental solver. #328 (resume through calls) is in all
three.

The reviewer's two programs, `--incremental-bmc --max-k-step 8`, 3 runs each
(every run ends UNKNOWN at bound 8):

| program | main (resume) | noresume | noresume `--incremental-solver` |
|---|---|---|---|
| fz-gen2/p123.c | 33.2 s [31.0-41.2], 379 MB | 15.7 s [15.7-17.3], 307 MB | 34.7 s [33.5-42.2], 314 MB |
| fz-gen2/p52.c | 83.2 s [77.2-84.9], 459 MB | 23.9 s [19.9-24.7], 198 MB | 82.2 s [72.9-91.4], 359 MB |

Resuming is 2.1 and 3.5 times slower than fresh walks, but exactly as slow as
fresh walks with the incremental solver: the loss is incremental solving.
Walking costs 0.18 s and 0.01 s in all, so resuming has nothing to save on
these programs. It does keep more memory (+23 % and +131 %).

The 13 `nla-digbench-scaling` tasks that motivated non-incremental solving in
#247, SV-COMP flags (`--incremental-bmc --property-file unreach-call.prp`, 60 s,
3 runs; walk time is 0.00 s on all):

| task | main | noresume |
|---|---|---|
| lcm1_valuebound5 | 30.3 s, 707 MB | 4.3 s, 221 MB |
| egcd3-ll_unwindbound10 | 2.05 s | 0.41 s |
| lcm1_valuebound2 | 2.18 s | 0.59 s |
| divbin_unwindbound10 | 9.25 s | 4.73 s |
| prodbin-ll_valuebound50 | 9.37 s | 4.34 s |
| prodbin-ll_valuebound100 | 23.9 s | 12.6 s |
| hard-ll_valuebound100 | 11.0 s | 6.0 s |
| bresenham-ll_valuebound20 | 11.8 s | 9.4 s |
| egcd3-ll_valuebound20 | timeout | 50.0 s |
| freire1_valuebound5 | 24.0 s | 34.8 s |
| prod4br-ll_valuebound20 | 5.9 s | 10.8 s |
| fermat2-ll_valuebound20 | 10.2 s | 31.9 s |
| knuth_valuebound50 | 13.3 s | 43.4 s |

Every spread was under 10 %. Resuming lost on 9 and won on 4; the totals are
equal (213 s each, main with one more timeout).

A random sample of 60 ReachSafety unreach-call tasks (loops, loop-*, array-*,
nla-digbench, ntdrivers-simplified, ldv-regression; seed 1), SV-COMP flags,
one run of main, noresume, nobl and rewalk each: 35 tasks ended in time for
both; the totals were 1538 s and 1542 s (timeouts counted as 60 s). The eight
tasks that differed by more than 20 % and 0.3 s, run 3 more times:

| task | main | noresume | where the time goes |
|---|---|---|---|
| array-crafted/mapsum2 | timeout | 0.28 s | main walks bound 1001 in 0.04 s, then Bitwuzla does not return in 60 s |
| loop-new/gauss_sum.i.v+cfa-reducer | 35.6 s | timeout | solve |
| array-programs/partial_mod_count_limited_3 | 2.97 s | 4.29 s | solve |
| loop-acceleration/diamond_1-2 | 0.17 s | 0.49 s | solve |
| array-examples/data_structures_set_multi_proc_trivial_ground | 3.16 s | 7.39 s | walk 3.0 against 6.9 s |
| array-examples/standard_running-1 | 15.6 s | 22.5 s | walk 8.9 against 13.9 s |
| array-examples/standard_compareModified_ground | out of memory at 5 GB, 57 s | timeout | |
| array-examples/standard_partial_init_ground | out of memory at 4.4 GB, 37 s | timeout | |

And 40 random fz-gen2 programs (seed 2) with the reviewer's flags: totals
13.6 s and 12.6 s, all under 4 s; three differed, confirmed over 3 runs: p120
0.36 against 1.01 s and p204 0.81 against 1.69 s for main, p208 3.60 against
0.91 s for noresume.

So resuming is not a net loss in general; it is a coin toss on programs where
solving dominates, because there the choice is really between Bitwuzla's
incremental and non-incremental modes, and it wins where walking dominates
(the array-examples tasks). The large losses (p52, lcm1_valuebound5, mapsum2,
egcd3-ll_valuebound20) all have a near-zero walk. Resuming also lets memory
grow across bounds: the two tasks that time out with fresh walks run out of
memory with it.

## 2. A 16-bit table with one extra write

`u16_N.c` fills `unsigned short a[N]` with `i * 3` in a loop, then does
`a[N-1] ^= 1` (a 16-bit load, xor and store in the IR) and checks `a[k]` at a
nondet `k`; `u16ok_N.c` leaves out the extra write. Main, 3 runs, 300 s,
4096 MB:

| N | u16 default flags | u16 `--no-summarize-array-loops` | u16ok `--no-summarize-array-loops` |
|---|---|---|---|
| 1000 | 0.13 s | 3.77 s [3.69-3.88], 373 MB | 0.14 s, 42 MB |
| 5000 | 0.13 s | out of memory after 85 s [83-86] | 0.16 s, 46 MB |
| 20000 | 0.12 s | out of memory after 41 s [41-42] | 0.26 s, 61 MB |
| 70000 | 0.12 s | out of memory after 37 s | 0.60 s, 107 MB |

With the default flags the fill loop is summarised and nothing is slow. With
`--no-summarize-array-loops` the claim holds: without the write the table
compresses and stays under a second; with it the table goes to the solver
whole and runs out of memory from N = 5000 (the review's 120 s timeout at
5000 was this run under 8 GB). The walk takes 0.06 s; Bitwuzla takes the
memory.

## 3. #241 and esbmc/github_302

| binary | wall | solve | peak MB |
|---|---|---|---|
| p241b | 17.4 s [17.0-18.0] | 17.3 s | 557 |
| p241h | 27.2 s [25.3-28.4] | 27.0 s | 756 |
| main | 6.6 s [6.5-6.8] | 6.4 s | 266 |

9 runs each for the pair, 4 for main. The claim holds: #241 made github_302
56 % slower and 36 % larger, which its survey put down to Bitwuzla's search
taking another path once 12 unused `bases` assertions went away. Later
changes more than undid it; nothing to file.

## 4. #276 table compression

p276b against p276h, 3 runs, 120 s. The tests #276 cites, then tables of
1024 elements (4096 where named) with periods 3 to 15, either filled by a loop
(run with `--no-summarize-array-loops`) or given as a constant initializer,
`a[i] = (i % p) * 7 + 1`, read at a nondet index:

| case | p276b | p276h |
|---|---|---|
| esbmc/tuple_const_array_struct_member | 33.6 s [31.4-34.7], 541 MB | 0.15 s, 45 MB |
| esbmc/tuple_const_array_struct_member_fail | 0.17 s | 0.14 s |
| esbmc/tuple_const_array_member_reads_fail | 32.2 s, 2872 MB | 32.1 s, 2869 MB |
| esbmc/tuple_const_array_member_reads (KNOWNBUG) | out of memory | out of memory |
| loop, period 3 | 14.9 s, 273 MB | 0.16 s |
| loop, period 5 | 36.4 s, 598 MB | 0.17 s |
| loop, period 7 | 31.7 s, 531 MB | 0.16 s |
| loop, period 15 | 45.1 s, 570 MB | 0.19 s |
| loop, period 3, 4096 | timeout | 0.18 s |
| initializer, period 3 | 56.2 s, 559 MB | 0.21 s |
| initializer, period 5 | 107.4 s, 814 MB | 0.21 s |
| initializer, period 7 | one of 3 within 120 s | 0.24 s |
| initializer, period 15 | timeout | 0.24 s |
| initializer, period 3, 4096 | timeout | 0.24 s |

The claim holds, by two orders of magnitude, on tables whose period is not a
power of two; tuple_const_array_member_reads_fail is not one of them.

## 5. #240 solver, witness and driver changes

p240b against p240h, 5 runs:

| case | p240b | p240h |
|---|---|---|
| esbmc/github_302 | 26.5 s [25.8-26.8], 534 MB | 26.3 s [26.0-27.0], 536 MB |
| esbmc-unix2/23_picosat-846_01 | 12.8 s [12.5-13.5], 379 MB | 12.8 s [12.4-14.4], 385 MB |
| vereda/lib_strtok | 6.06 s [5.57-6.52], 108 MB | 6.03 s [5.58-6.28], 111 MB |
| array_loop_summary_decrement_fail `--no-summarize-array-loops` | 5.58 s [5.05-6.51], 630 MB | 4.90 s [4.64-6.42], 629 MB |

Noise. The last row's medians differ by 0.7 s, but its walk (1.10 / 1.08 s)
and solve (3.38 / 3.33 s) do not, and the ranges overlap. Freeing the module
before solving gives no measurable peak-memory change on these tests.

## 6. #238 Terms tables and fcmp NaN checks

p238b, p238nan (parent plus only the fcmp change) and p238h, 5 runs:

| case | p238b | p238nan | p238h |
|---|---|---|---|
| esbmc/github_302 | 26.2 s [25.3-27.3] | 17.5 s [17.3-17.7] | 17.2 s [16.8-17.8] |
| esbmc-unix2/23_picosat-846_01 | 9.13 s, walk 0.48 | 9.21 s, walk 0.48 | 8.70 s, walk 0.39 |
| array_loop_summary_nondet `--no-summarize-array-loops`, walk | 4.80 s [4.64-4.93], 534 MB | 4.88 s, 534 MB | 3.93 s [3.75-3.97], 519 MB |

Peak memory on github_302 was 537, 558 and 556 MB. The claim holds with
repeats: github_302 is 34 % faster, and the fcmp change alone gives all of it.
The Terms rewrite, the other half of the commit, does nothing on github_302
and cuts walk time by 18 % (and 15 MB) on the walk-heavy test, as the commit
also said.

## 7. The rewalk in `decideNonIncremental` (#247)

The rewalk binary prints the time of each fresh walk that
`decideNonIncremental` makes (a second phase after the first held, or the
remaining checks under `--multi-property`). That time lands in the `Solving
time` line, not the `Walk time` line.

Over the whole suite's CORE and THOROUGH tests (3,252, first RUN line, one run
each), 59 tests rewalked, 86 times in all (45 once, 9 twice, at most 6 times),
for 0.12 s in total, against 0.10 s of reported walk time and 12.4 s of
solving in those tests; the largest single rewalk took 17 ms
(tuple_const_array_member_reads_fail). On the 60 SV-COMP tasks under
`--incremental-bmc` no bound rewalked. So the walk line does under-report, by
about the walk time again, in 2 % of the tests, which is too little to matter
today; a test with a slow walk and a second phase would pay it in full.

## 8. `boundedLimit` under `--incremental-bmc` (#281)

nobl against main on the same 60 SV-COMP tasks: totals 1538.3 s each; the 35
tasks both finished walked 12.20 and 12.20 s; no task moved by 20 % and 0.3 s.
On a program built to call `boundedBackedges` often (three small constant
inner loops in a nondet outer loop, `--max-k-step 1025`, 5 runs): 0.32 s
[0.31-0.32] and 0.31 s [0.31-0.34], walk 0.10 s each. The work is real but
too small to measure.

## 9. The pull requests #330, #335 and #336

All three against their merge base, first on the whole suite (3,252 CORE and
THOROUGH tests, one run each, interleaved), then 5 runs on whatever moved by
30 % and 0.5 s.

Suite totals: mb 554.9 s, pr330 552.2 s, pr335 549.8 s, pr336 553.3 s; no
verdict changed and no test grew by 30 % and 50 MB. Three tests moved in that
one run; over 5 more runs vereda/reach_error_body and vereda/reach_false took
0.12 to 0.13 s on all four binaries (the single slow run was mb's, an
outlier), and esbmc/github_7548_getline_reuse took 0.95 s on mb, pr335 and
pr336 and 0.40 s [0.40-0.40] on pr330.

#330 (one-path layout), esbmc-unix2/23_picosat-846_01, 5 runs: mb 1.17 s
[1.12-1.18], 129 MB; pr330 0.83 s [0.81-0.83], 102 MB (solve 0.57 against
0.22 s); the single suite run agreed (1.21 s, 129 MB against 0.81 s, 102 MB).
The pull request's own 0.71 → 0.82 s and 80 → 102 MB do not reproduce: on
this machine the base is the slower and larger one. Verdict: no regression.

#336 (lowest failing check), 60 random expected-false unreach-call tasks from
the same directories (seed 3), SV-COMP flags, one run: totals 731.8 s and
722.0 s, 47 FAILED and 1 UNKNOWN each, 8 timeouts and 4 out of memory on mb
against 7 and 5 on pr336. Five tasks
moved; over 3 more runs four took 0.12 to 0.13 s on both binaries (the first
run of each task read a cold `.i` file), and
array-examples/sorting_bubblesort_2_ground ran out of 6 GB or timed out on
both. Verdict: no measurable cost on ReachSafety; the extra queries only run
in `decideNonIncremental`, which SV-COMP's `--incremental-bmc` reaches on
bounds without a checkpoint.

#335 (copy tracking in the walker), default flags: the suite run above shows
nothing; its 8 slowest tests, 3 runs each, matched within 2 % (github_302
6.36 / 6.37 s, the two deep_binary_chain tests 19.4 / 19.4 and 19.1 / 19.0 s,
mem_store_chain_reads 9.95 / 9.81 s, lib_strtol 6.18 / 6.07 s, the same peak
memory). That batch ran once at a load of 34 from other jobs, wider but also
without a difference, and was repeated at a load of 9 for the figures here.
With `--ub-uninit-check` on the vereda, esbmc-unix and esbmc-unix2 suites (648
tests, one run): totals mb 1650 s and pr335 1525 s, the same verdicts except
vereda/mem_data_read_then_pointer_large, which times out at 120 s on mb and
passes in 0.61 s [0.61-0.62] on pr335 (3 runs); lib_strtok, lib_strtol,
mem_store_chain_reads and summary_columns matched within 4 % over 3 runs.
Verdict: no cost on the default path; faster with the uninit check.

## Notes

Single screening runs had occasional outliers, mostly the first run of a
file (cold page cache), so every difference quoted above comes from repeats.
A process of another user ran on core 35 throughout; with at most 8 runs on
35 cores it did not compete.

## Draft follow-ups

Not filed; left to the maintainer. Resuming under `--incremental-bmc` ties
two things together, keeping the walk and switching Bitwuzla to its
incremental mode, and on solve-dominated programs the second decides the time
(item 1); one option is to resume only when walking took a noticeable share
of the last bound's time. A table filled by an unsummarised loop and then
written once more is not compressed and runs out of memory from 5,000 16-bit
elements (item 2).
