# Incremental BMC

Recorded 2026-11-22.

`--incremental-bmc` checks bounds 1, 2, ... up to `--max-k-step` (50) in one
process and stops at the first verdict that does not depend on the bound:
a violated assertion, or every unwinding assertion holding. The plan
(section 4.2) asks to keep the walk's state at bound k and continue from it.
This walker merges the paths that leave a loop, so a larger bound changes
the state after the loop and everything from there on is walked again;
ESBMC's incremental BMC also reruns symbolic execution per bound. Each bound
gets a new walk and solver; the IR is loaded and prepared once.

Wall time for bounds 1 to K, three runs side by side:

| Task | K | One process per bound | One process, new solver per bound | One solver, a scope per bound |
|---|---|---|---|---|
| Problem08 | 3 | 129 s | 54 s | 44 s |
| Problem102 | 3 | 119 s | 54 s | 45 s |
| uthash | 2 | 63 s | 32 s | 31 s |
| SpamAssassin | 12 | 43 s | 41 s | 61 s |
| bAnd4 | 20 | 0.40 s | 0.11 s | 0.05 s |

The saving over one process per bound is the IR preparation (the RERS
modules are 123 MB of IR). Keeping one solver and one term builder for all
bounds, with each bound's constraints in a push/pop scope, saved 17% on the
RERS tasks but cost 50% on SpamAssassin (61 s against 40 s, twice each), so
it was not kept.

On the verdict check's 188 distinct tasks (one is in both lists) at
`--max-k-step 10`, 120 s each: 4 FAILED and 4 SUCCESSFUL, all as expected;
139 UNKNOWN with no verdict up to bound 10, 9 unsupported, 32 timeouts. No
wrong verdict.
