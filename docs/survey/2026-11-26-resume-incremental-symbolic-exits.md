# Resuming incremental BMC across symbolic loop exits (#175)

## Background

PR #245 introduced `walker::Incremental` to resume an incremental-BMC walk across
bounds when a top-level loop in the entry function is unconditional (`t.isTrue(reached)`),
its header is the only pending arrival, and there are no early returns. Any loop with a
symbolic exit condition (`while (i < n)`), early return, or sibling branch fell back to
a full walk from scratch on every bound, and in default (non-incremental) solver mode
`decideNonIncremental` also reset `BMCState` whenever a non-constant assertion was solved.

## Design

### Suffix-rollback checkpoints in `Walk`

When a top-level loop in `main` hits `opts.unwind` with `!t.isFalse(reached)`, no earlier
checkpoint active, and no truncated inner loop/callee/recursion (`!truncatedInner`):

- **Fast path (`immediate = true`):** unchanged from #245 when `frame.arrivals.size() == 1`,
  `frame.returns.empty()`, and `t.isTrue(reached)` — nothing after the loop is reachable.
- **General top-level loop (`immediate = false`):** before emitting the unwinding check,
  `Walk` snapshots `frame` (persistent `Trie`s for `Memory::State` and `Encoder::Facts`),
  `checks.size()` and each prefix check's `violated` term (in case a shared callee is also
  called in the suffix), `trace.size()`, claim/assertion/recursion numbering maps,
  `summarised`, `t.constraintsEpoch()`, and `mem.objectCount()`. The live `frame` clears
  the loop header and continues walking the post-loop suffix so bugs reachable within
  bound `k` are still checked.
- At the end of the walk (`finalizeStopped()`), if the suffix added any global solver
  constraints or allocated objects (`constraintsEpoch` or `objectCount` changed), the
  checkpoint is discarded and the next bound starts afresh.
- On `advance(f, bound)` for bound `k' > k`, the suffix is rolled back (truncating
  `checks` and `trace`, restoring prefix `violated` terms and claim/assertion numbering),
  and the loop resumes from iteration `k` with all previously accumulated exit arrivals
  intact.

### Soundness guard for nested truncations

`Walk` tracks `truncatedInner`: whenever any non-checkpointed cut occurs (an inner loop,
a loop inside a callee, or recursion unwinding with `!t.isFalse(guard)`), `truncatedInner`
is set so an enclosing outer loop cannot save a checkpoint containing truncated inner
iterations.

### Solver reuse when resumable

In `src/driver/main.cpp`, when `state->resumable()` is true under `--incremental-bmc`,
the driver uses `solver::decide` (`checkSatAssuming`, which does not add permanent
assertions to the solver) instead of `decideNonIncremental`, and creates the solver with
`incrementalSolver || incrementalBMC` so Bitwuzla's `bv-cnf-scope=permanent` keeps
shared prefix AIG clauses across bounds. Once the final bound completes without a
checkpoint, `walk`, `encoder`, and `memory` are freed before solving as before.

## Measurements

LLVM 23.1.6, camada 0.25 / Bitwuzla 0.11.0, `--no-summarize-array-loops --incremental-bmc --unlimited-k-steps`:

| Case | Main (`5a04176d`) | Branch | Resumes | Verdict |
|---|---|---|---|---|
| `symbolic_exit_200_true` (SCEV-bounded) | 0.32 s | 0.20 s | 8 | SUCCESSFUL |
| `symbolic_exit_150_false_suffix` (SCEV-bounded) | 0.24 s | 0.15 s | 8 | FAILED |
| `symbolic_exit_early_return` (SCEV-bounded) | 0.19 s | 0.13 s | 7 | SUCCESSFUL |
| `linear_nondet_exit_150_true` | 0.35 s | 0.16 s | 8 | SUCCESSFUL |
| `linear_symbolic_step_100_true` (non-SCEV, 100 bounds) | 3.53 s | 1.00 s | 100 | SUCCESSFUL |

## Regression coverage

- Full `vereda` suite (including updated `incremental_resume_exits.c` and new tests
  `incremental_resume_exits_false.c`, `incremental_resume_exits_multi.c`,
  `incremental_resume_nested_exit_sound.c`, and `incremental_resume_suffix_alloc.c`).
- All 279 imported ESBMC `--incremental-bmc` tests pass with no regressions.
