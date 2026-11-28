# Defer unwinding-only queries near known loop bounds

PR #419 changes the default from plain BMC to incremental BMC. The remaining
`esbmc/github_302` regression is an expensive query about a truncated loop,
not an expensive final program assertion. This investigation uses current
main `6a913fef` and #419's merged head `aec7262f`, LLVM 23.1.6,
RelWithDebInfo, mimalloc 3.7.0 and the same pinned camada 0.26 installation.
The input IR is shared between all runs.

## Isolate renewal from the query interface

`--fresh-solver` changes both backend renewal and the driver's choice of
assumption-based versus direct solving at resumable bounds. Temporary probes
separated those choices; none of the probes remains in production code.

| Policy | Total wall seconds |
| --- | ---: |
| Keep the instance and use assumptions, the #419 default | 23.23 |
| Renew before each bound but retain assumption-based solving | 33.43 |
| Keep the first instance but use direct checks | 13.62 |
| --fresh-solver, which renews and uses direct checks | 13.62 |

Bound 1 makes no solver call: its constant unwinding failure is handled
without a model. Bound 2's first real query is an assumption-based check of
a symbolic unwinding condition. It takes about 21 seconds; the direct
version takes about 2 seconds. Keeping the queried backend then helps the
final bound, which takes about 2 seconds instead of about 11 seconds on a
fresh instance. These measurements do not support the proposed explanation
that a symbol-free bound-1 query consumed first-query preprocessing.

The earlier current-main survey's execution description was incomplete:
the traced incremental run reaches bounds 1, 2 **and 3**. Bound 3 contains
the failing program assertion.

## Avoid a query whose only purpose is choosing a larger bound

When every pending program property already folds to false (no violation),
only unwinding checks remain. If each open loop has a known finite SCEV
bound, the driver may conservatively extend the walk instead of asking the
solver whether to extend it. This does not declare any check safe, add an
assumption, or discard any solver state. The extended walk still checks
program properties and the final unwinding conditions normally.

Deferral is deliberately limited. The next known loop bound must fit within
the next doubling step and within the configured maximum, accounting for
`--k-step`'s grid. Unknown trip counts, recursion without a finite bound,
live program checks and multi-property runs retain their existing query
path. A distant finite bound still gets a feasibility query: otherwise an
infeasible path could make an unlimited run walk a billion iterations
instead of finishing after one cheap proof. At the largest available bound,
a query can still prove the cut unreachable and establish safety.

For github_302 this skips the bound-2 unwinding query and reaches the final
assertion at bound 3 without first training the backend on the cut loop.
No heuristic about symbol counts or formula-size ratios is needed.

## Paired measurements

Three interleaved runs per binary on the same pinned CPU give these medians:

| github_302 | Wall seconds | Peak RSS KiB |
| --- | ---: | ---: |
| Main 6a913fef, default plain BMC | 12.01 | 466100 |
| #419 before deferral | 24.13 | 520168 |
| #419 with deferral | 12.11 | 468544 |

All nine executions report the same expected violation. The fix removes
about half of the PR's runtime and returns it to main's time and memory.

A separate interleaved before/after pair on one dedicated CPU checks
32_floppy with `--unwind 1 --ub-pointer-check=false
--ub-div-by-zero-check=false --error-label ERROR`, the same saved IR and a
6 GiB address-space limit:

| 32_floppy | Before | After |
| --- | ---: | ---: |
| Total wall seconds | 63.36 | 62.86 |
| Walk seconds | 45.929 | 45.963 |
| Solve seconds | 15.168 | 14.686 |
| Peak RSS KiB | 1047324 | 1045608 |
| Verdict | FAILED | FAILED |

The #452 preservation benefit remains. These measurements are local runs
on a shared host, not a complete BenchExec comparison.

## Validation

Five new fixtures have nine configurations: normal, incremental-solver and
fresh-solver execution; a non-unit k-step; a live assertion in the first
iteration; an unknown loop bound; a distant finite bound; and configured
limits both on and between the k-step grid points. The limit fixtures prove
an infeasible cut safe rather than returning UNKNOWN after a deferred query.
The full Vereda suite has 928 passes and one existing skip, with no failures.

CI on the main merge also exposed six imported fixtures whose default mode
changed: four github_7597 chain tests require 1000 or 1025 iterations, and
two intrinsic_unroll tests expect pragma bounds in plain BMC. They now
explicitly select `--no-incremental-bmc`, retained by the importer's existing
preservation table. Their bodies and expectations are untouched. Targeted
execution gives two passes and the four existing FUTURE skips, no failures.

The broader comparison retains the existing #452 symbolic-size-query policy.
Gauss, sum01-2, Geo and Freire retain their verdicts and similar CPU times;
Prod and LCM reach the CPU limit in both builds. No deferral occurs in these
cases, so their shared-instance behavior is retained. The egcd wall-limited
comparison was inconclusive because its CPU was shared with another run;
it is not used as performance evidence. Full ReachSafety Arrays, ControlFlow,
Heap and Loops comparisons remain necessary for broader coverage.

Follow-up issues: #387 (per-bound solver policy and broader comparisons),
#452 (useful walk-query history).
