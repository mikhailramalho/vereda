# Non-incremental solving as default with `--incremental-solver`

Recorded 2026-11-25 against `main` at `f803a4cc` (LLVM 23.1.6, camada 0.25 /
Bitwuzla 0.11.0).

## Motivation

Comparing Vereda against ESBMC on SV-COMP ReachSafety Loops (run 45603723886)
showed that all 12 tasks ESBMC solved while Vereda timed out—and 20 of the 22
tasks where ESBMC was faster—were nonlinear 64-bit bitvector tasks in
`nla-digbench-scaling/` with 0.00–0.01 s walk time.

Testing standalone Bitwuzla on Vereda's own `prodbin-ll_valuebound50` (`k = 7`)
formula isolated the cause: Vereda's formula solved in 2.71 s with
non-incremental `(check-sat)` (faster than ESBMC's 3.61 s), but slowed to
7.42 s under `(check-sat-assuming ...)` and 7.51 s under `(push 1)`. Calling
`checkSatAssuming` switches Bitwuzla into incremental mode, disabling
non-incremental preprocessing passes.

## Change

Default solving is now non-incremental (`decideNonIncremental` in
`src/solver/solver.cpp`):

- `solver::create(kind, /*incremental=*/false)` uses Bitwuzla's default
  options (`camada::createBitwuzlaSolver()`) instead of
  `bv-cnf-scope=permanent`.
- Each phase (assertions, then unwinding assertions) asserts the disjunction of
  its deduplicated non-constant checks via `addConstraint` and calls `check()`
  once without assumptions or `push`/`pop`.
- Constant-true unwinding assertions under `--incremental-bmc` return
  immediately without touching the solver, keeping PR #245's resumable prefix
  intact when no assertions need solving.
- When a second query is required (symbolic unwinding assertions after
  assertions held, or remaining undecided checks under `--multi-property`),
  `bmc()` re-walks into a fresh non-incremental solver.
- `--incremental-solver` restores the previous one-solver `checkSatAssuming` +
  `bv-cnf-scope=permanent` path for side-by-side comparison.

## Measurements

Local runs on the 34 `nla-digbench-scaling`/loop tasks where ESBMC beat Vereda
in CI (60 s timeout, 12 workers, vs local ESBMC 8.3.0 `-s incr` and Vereda
`f803a4cc`):

| Task | ESBMC 8.3.0 | Vereda (incremental) | Vereda (non-incremental) |
|---|---:|---:|---:|
| `nla-digbench-scaling/prodbin-ll_valuebound100.yml` | 21.62 s | TIMEOUT | **12.53 s** |
| `nla-digbench-scaling/prodbin-ll_valuebound50.yml` | 10.18 s | 39.58 s | **4.46 s** |
| `nla-digbench-scaling/lcm1_valuebound5.yml` | 5.43 s | 29.47 s | **5.13 s** |
| `nla-digbench-scaling/divbin_unwindbound10.yml` | 4.74 s | 24.66 s | **4.83 s** |
| `nla-digbench-scaling/egcd3-ll_unwindbound10.yml` | 1.11 s | 2.53 s | **0.42 s** |
| `nla-digbench-scaling/egcd3-ll_valuebound20.yml` | TIMEOUT | TIMEOUT | **47.89 s** |
| `nla-digbench-scaling/freire1_valuebound5.yml` | 29.88 s | TIMEOUT | **34.01 s** |
| `nla-digbench-scaling/knuth_valuebound50.yml` | 35.08 s | TIMEOUT | **45.36 s** |
| `nla-digbench-scaling/bresenham-ll_valuebound20.yml` | 28.63 s | 50.08 s | **27.89 s** |
| `nla-digbench-scaling/fermat2-ll_valuebound20.yml` | 42.65 s | 48.63 s | **33.58 s** |
| `nla-digbench-scaling/prod4br-ll_valuebound20.yml` | 19.66 s | 20.35 s | **11.24 s** |
| `nla-digbench-scaling/hard-ll_valuebound100.yml` | 9.58 s | 10.37 s | **6.33 s** |
| `nla-digbench-scaling/lcm1_valuebound2.yml` | 1.37 s | 1.74 s | **0.58 s** |
| `loops-crafted-1/theatreSquare.yml` | 37.91 s | TIMEOUT | **51.88 s** |

Full `ctest -j75` suite: 7,008 tests (0 failures after adding
`--multi-property` to `vereda/assume_constant_float_nan_fail`, which checks
both a passing assertion and a later failing assertion).
