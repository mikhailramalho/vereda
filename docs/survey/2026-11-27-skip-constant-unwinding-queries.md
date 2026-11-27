# Skipping constant unwinding queries under --incremental-bmc

Recorded 2026-11-27.

Under `--incremental-bmc`, a bound whose walk resumes asks the one retained
solver about each check with `checkSatAssuming` (`solver::decide`). At early
bounds the unwinding assertions often fold to true, and asking Bitwuzla about
them anyway costs more than the trivial call: Bitwuzla runs its normalization,
skeleton preprocessing and full variable substitution only on an instance's
first check, so the real query at a later bound is solved without them. On
`reducercommutativity/sum05-2` the formula at bound 7 is two sums of the same
five values in different orders; Bitwuzla proves them equal in 0.001 s as its
first query, but takes about 40 s after two trivial queries at bounds 1 and 3
(#380, cause 1). `decideNonIncremental` already skipped these checks; `decide`
now does the same: a constant failed unwinding assertion fails without a
solver call.

## Measurements

Main at db722441 against the branch, each run once with the SV-COMP flags
(`-m32 --incremental-bmc --property-file unreach-call.prp`, which adds
`--unlimited-k-steps --k-step 2`), 60 s and 6 GiB of address space per run,
both binaries side by side on cores 0-39 of the 80-core machine, at most 12
runs at a time. Seconds, wall time of `vereda.py`.

| task | main | branch |
| --- | --- | --- |
| reducercommutativity/sum05-2 | 39.6 | 0.21 |
| reducercommutativity/sep60-2 | 23.5 | 0.21 |
| reducercommutativity/sep40-1 | 5.8 | 0.21 |
| reducercommutativity/avg05-1 | timeout | 0.21 |
| reducercommutativity/sum10-1 | timeout | 0.21 |
| reducercommutativity/sum20-2 | timeout | 0.21 |
| reducercommutativity/sum40-2 | timeout | 0.21 |
| reducercommutativity/sum60-2 | timeout | 0.21 |
| array-crafted/mapsum1 | timeout | 0.21 |
| array-crafted/mapsum2 | timeout | 0.32 |
| array-examples/standard_vector_difference_ground (3 runs) | 4.9-5.1 | 6.3-6.5 |

Every answer is TRUE, as expected. `standard_vector_difference_ground` is the
one task #380 found slower among the 75 others it measured: its last solve,
on a 200,000-symbol formula, now gets the full preprocessing as a first query
and takes longer than in incremental mode.

The test `vereda/incremental_resume_preprocessing` (sum05-2 cut down to its
loops) takes 39 s on main and 0.13 s with the change; `vereda/solver_verdicts`
checks that no solver call is made.

This matters only while one solver serves every bound: with a fresh solver per
phase (branch `feat/fresh-solver-per-phase`, waiting for a camada release)
it matters only under `--incremental-solver` (#388).
