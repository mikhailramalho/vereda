# Solver renewal against current main

Merged main `6a913fef` into PR #419 before continuing the performance
investigation. Main now numbers unwinding claims in the assertion namespace,
supports per-loop bounds and pragma bounds, and checks native solver errors.
The merge retains those behaviors, treats explicit per-loop bounds as plain
BMC in both the driver and property-file front end, and keeps the plain-mode
pragma regression explicit. The new per-loop default-mode regression runs
both with and without a property file.

The merged tests needed updated property IDs. The new snprintf long-double
regression also needs plain BMC: its modeled formatting loop can exceed the
384-iteration default. Its two data-model configurations retain their original
unbounded mode rather than changing the default bound for every program.

## github_302

Paired binaries use LLVM 23.1.6, RelWithDebInfo and the same renew-capable
camada developer installation (which reports 0.25), including the same native
solver libraries. They consume the same saved LLVM IR for
`tests/esbmc/github_302.c`. Runs are pinned to one CPU with a 4 GiB address-space
limit; the following are single-run measurements, not medians.

| Configuration | Wall seconds | Peak RSS KiB |
| --- | ---: | ---: |
| Current main, default plain BMC | 11.92 | 461380 |
| Merged #419, default incremental BMC | 23.83 | 492208 |
| Merged #419, --no-incremental-bmc | 12.21 | 462376 |
| Merged #419, --fresh-solver | 14.82 | 475644 |

The regression therefore survives the main update and is associated with the
changed default execution mode; the explicit plain mode remains close to main.
The incremental run stops at bound 1 without querying the solver and resumes
at bound 2. Temporary query logging confirmed this; that logging was removed.

An attempted change to use direct solving when a saved continuation has no
remaining open unwinding checks took 24.73 seconds and 533468 KiB, so it was
rejected and removed. This is not evidence for changing the general policy:
keeping symbolic-size query history still matters for #452, and renewing
across all bounds already has mixed results recorded in the original survey.
The performance regression remains unresolved.

Follow-up issues: #387 (solver policy across bounds), #452 (useful walk-query history).
