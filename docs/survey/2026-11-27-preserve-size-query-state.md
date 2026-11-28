# Preserve symbolic-size query state before solving

Recorded 2026-11-27.

PR #419 renewed Bitwuzla before every solving phase in plain BMC. Issue #452
showed that this discards useful search state left by symbolic memory-size
queries: `esbmc/32_floppy` then spends much longer solving its properties.

The first automatic renewal now preserves the instance if `Walk::single()`
or `Walk::rangeBound()` actually needs to query it. Constant sizes and bounds
rejected before a solver call do not request preservation. The marker is
kept in `Terms`, which outlives the walker and the walk-table release.
`--fresh-solver` explicitly overrides preservation. Later solving phases and
the renewal needed to resume after an asserted property still renew
unconditionally, so a previous phase's violation assertion cannot leak into
another phase or bound.

This is deliberately narrower than preserving every queried instance.
The initial experiment also kept loop-feasibility queries; it restored
`32_floppy` but regressed Dijkstra from 3.01 s to 8.91 s and LCM from 2.61 s
to 3.81 s. Their cheap feasibility queries benefit from the fresh final
instance. That blanket policy was rejected. Feasibility queries therefore
retain #419's existing renewal policy; size queries explicitly mark their
state for preservation.

## Measurements

Both builds start from PR #419 at `5c0a8e1d`, with LLVM 23.1.6,
RelWithDebInfo, mimalloc 3.7.0 and the same renew-enabled camada install used
by the PR (its package version reports 0.25), with Bitwuzla 0.11.0. Runs are
pinned to one CPU, with a 6 GiB address-space limit. These are single paired
measurements on a shared machine, not benchmark medians.

`32_floppy` uses its unchanged C fixture, compiled once for the 64-bit host
with division instrumentation disabled, and both binaries use that same IR:

```
--unwind 1 --ub-pointer-check=false --ub-div-by-zero-check=false --error-label ERROR
```

| `32_floppy` | Before | After |
| --- | ---: | ---: |
| Walk | 51.98 s | 54.18 s |
| Solve | 153.01 s | 17.64 s |
| Total wall | 207.80 s | 74.27 s |
| Peak RSS | 1,641 MiB | 1,009 MiB |
| Verdict | FAILED | FAILED |

The SV-Benchmarks fixtures are from revision
`5092e58a0044330c449b05177bb13027081af369`, under
`c/nla-digbench-scaling`, compiled with `-m32`. Both binaries use
`--property unreach-call --no-incremental-bmc`; the compiled IR is shared.
All six runs report SUCCESSFUL.

| Task | Before total wall | After total wall |
| --- | ---: | ---: |
| `dijkstra-u_valuebound100` | 3.01 s | 2.91 s |
| `lcm1_valuebound5` | 2.61 s | 2.41 s |
| `knuth_valuebound100` | 57.06 s | 54.05 s |

The initial blanket experiment with explicit `--fresh-solver` took
56.03 s walking and 151.33 s solving `32_floppy`, confirming that forcing
a fresh instance retains the slow behavior. The focused unit test verifies
the override directly, without depending on performance timings.

## Verification and remaining work

`vereda/solver_verdicts` checks marked and unmarked queries, including SAT,
UNSAT and UNKNOWN results, preservation after walk-table release, forced
renewal, marker reset and reassertion of global constraints over live term
handles. `solver_renew_after_size_query.c` proves a symbolic-length copy
property, then finds a failing unwinding property, both with normal renewal
and with `--fresh-solver`. Keeping the first phase's asserted violation for
the second would incorrectly hide that unwinding failure.

The complete Vereda suite passes: 734 passed and one existing skip
(`vereda/lib_sprintf_overflow`), with no failures.

This preserves the useful history identified by #452 without changing SAT
options or adding artificial warm-up queries. It is not a general optimal
renewal policy: #452 still tracks separating learned clauses, phases and
theory state, and #387 tracks choosing between fresh and retained instances
across bounds.

Follow-up issues: #452, #387
