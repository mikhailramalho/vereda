# Guard folding in the term builder

Recorded 2026-11-23.

The walker builds a guard as `land(parent, condition)`, a chain of ands down
the left side, and joins arrivals with `lor`. `Terms::land` and `lor` folded
only at the top level, so a condition tested twice on one path, or an
if/else diamond joining back, reached the solver as a longer formula. ESBMC
keeps a guard as a list of conjuncts with a cached and-chain, and finds the
shared prefix of two guards by walking down that chain (`irep2_guard.cpp`,
`guard_seq.h`, and esbmc commits 4fb875f1a1, a0191ec2c3, 77bf4678c1).

Kept, in `Terms`:

`land(g, c)` walks up to 64 steps of g's left spine: a conjunct equal to c
gives g, one that is not c (or whose negation is c) gives false. The second
prunes an edge while it is walked, so the blocks behind it are never
walked; it is ESBMC's `path_guard_decides`, bounded instead of indexed per
guard prefix, which needs no memory per guard.

`lor` factors one shared parent, as ESBMC's `|=` does in the case its
measurements say is the common one (suffixes of one conjunct): `(p and x)
or (p and y)` is `p and (x or y)`, which is p after a diamond, and `p or
(p and x)` is p.

Measured side by side with the base binary (min of two runs, 80-core
machine shared with other runs, walk + solve seconds, peak MB):

| test | base | head |
|---|---|---|
| esbmc/guard_merge_locks_perf `--unwind 300` | 0.08 + 0.15, 104 | 0.10 + 0.03, 92 |
| esbmc/guard_merge_locks_perf `--unwind 2000` | 0.66 + 1.14, 255 | 0.67 + 0.15, 170 |
| esbmc/promoted_unsigned_nonneg | 0.53 + 0.09, 164 | 0.30 + 0.00, 132 |
| esbmc/array_loop_summary_decrement_fail | 11.1 + 27.9, 2534 | 10.3 + 26.5, 2531 |
| esbmc/10_bicycle_01 | 0.03 + 7.23, 431 | 0.03 + 7.18, 431 |
| esbmc/11_seesaw | 0.00 + 1.05, 88 | 0.00 + 1.04, 87 |
| esbmc/10_fir_new | 0.19 + 0.19, 114 | 0.20 + 0.18, 114 |

On the locks test (SV-COMP's test_locks, the case ESBMC's guard work was
measured on) the error's check folds to false and the solver gets one check
instead of two. array_loop_summary_decrement_fail is unchanged: its loop
bound is constant, every guard is true and no block has two arrivals.

Over the 3,684 tests of the esbmc and vereda suites that finish within
20 s, the SMT that `--dump-smt-only` prints went from 237.8 MB to
216.7 MB; 174 tests changed, 172 smaller and two larger by at most 351
characters. 17.2 MB of the drop is esbmc/array_loop_summary_stride_fail,
where `p or (p and x)` folds the disjunction of its two checks into one; it
still runs past 150 s. No verdict changed against the base on the esbmc
suite (3,557 tests, 20 s each; one run past 20 s on a busy machine gives
the same verdict as the base side by side), and walk-bound tests
(esbmc/github_306, github_1289, github_1600) walk as fast as before.

Tried and dropped:

Factoring over the longest shared prefix, as ESBMC's `|=` does in full:
collect both left spines (64 steps), find where they meet, and rebuild
`p and (x or y)` from the conjuncts above it. The dumps hardly shrank
beyond what the one-parent folds give, but esbmc/10_bicycle_01 solved in
29.1 s instead of 8.2 s (613 MB instead of 432) and esbmc/github_1093 in
0.64 s instead of 0.38 s; esbmc/11_seesaw went from 1.07 s to 0.21 s. The
spine walks also doubled the walk time of the locks test at
`--unwind 2000` (0.74 s to 1.45 s). Rebuilt suffixes are new terms that
share nothing with the arrivals' guards, and the solver did worse on them.

Merging phi values and memory by each arrival's guard less what it shares
with the join's guard, as ESBMC's `-=` does for its phi functions:
`ite(c, x, y)` rather than `ite(p and c, x, y)` where p holds. It is sound
(values merged at a join are only read where the join's guard holds) but
changed little: vereda/mem_inttoptr solved in 2.05 s instead of 2.37 s,
esbmc/github_1093 in 0.52 s instead of 0.36 s, the rest within noise.

ESBMC's persistent conjunct list (`guard_seq`, O(1) copy) has no
counterpart to port: a guard here is one hash-consed term, already O(1) to
copy and to share.
