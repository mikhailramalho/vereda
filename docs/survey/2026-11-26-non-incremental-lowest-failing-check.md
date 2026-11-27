# Non-incremental solving reports the lowest failing check

Recorded 2026-11-26.

`decide()` (`--incremental-solver`, and every resumable bound under
`--incremental-bmc`) asks about each check in index order, so in
single-property mode it reports the lowest failing one.
`decideNonIncremental()` asserted the disjunction of the phase's checks and
reported whichever check the model happened to violate. The reported property,
and under `--incremental-bmc` the unwinding assertion whose `unwindsAt` picks
the next bound, then depended on whether the bound had a checkpoint. A fuzzed
program at `--unwind 2` reported `main.assertion.2` by default and
`main.assertion.1` under `--incremental-solver`; another tried bounds 1, 2, 4,
6 by default and 1 to 6 under `--incremental-solver`.

## Change

After a satisfiable phase, when the model's first violated check has
checks before it, `decideNonIncremental()` pushes a scope on the same solver,
asserts the disjunction of those earlier checks, and checks again, repeating
while the model shows an earlier one. The new disjunction implies the one
already asserted, so the query only narrows. When no earlier check can fail
(or the solver answers unknown), it pops that scope, asserts the check found,
and checks once more to get a model that violates it for the trace.

Re-walking into a fresh non-incremental solver, as the later phases do, was
not adopted: each extra query would have paid for a walk and a solve from
scratch, while the pushed scope reuses what the solver learned. Only these
extra queries run in Bitwuzla's incremental mode; the first one still gets the
non-incremental preprocessing measured in
`2026-11-25-non-incremental-solver.md`.

## Measurements

The extra query ran in 104 distinct test programs of the full suite (7,052
tests). Running the 319 test files with those names with the old and new
binaries, 10 at a time, took 83.9 s and 83.4 s in total; the slowest,
`esbmc-unix2/25_git-remote-gitkrb5`, took 11.3 s and 10.1 s, and none of the
others over 0.5 s changed by more than 0.03 s. The extra cost is proving that
the earlier checks hold, which a program whose failing check comes after many
hard ones pays in full, as `decide()` already does.
