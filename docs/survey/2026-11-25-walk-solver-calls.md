# Cutting walk-time solver calls and float bit round-trips

Recorded 2026-11-25.

Without `--unwind`, the walker calls `unreachable(reached)` at every loop
iteration `i > 0` to know when to stop unrolling. On two slow tests, those
walk-time solver calls dominated wall time:

- `esbmc/github_302` spent 36.5 s of 43.9 s walking because six tiny
  constant-count loops (`DIM = 2`, `n = 1`) ran under a symbolic guard
  containing `fp.sqrt` and `fp.div` assumptions from `rest_points`, issuing 14
  queries on the same guard term. In addition, storing floats into `aux1`/`aux2`
  and loading them back in `distance` built `fromBits(toBits(v, ty), ty)`,
  wrapping each loaded float in `to_fp(__CAMADA_ieeebv_fn0(v))`.
- `esbmc/github_9397` spent 3.43 s of 3.55 s walking because the floating-point
  conversion in `models/printf.c` unrolled `for (unsigned i = 0; i < k; ++i)`
  with nondet `k <= most`, issuing ~240 SAT `unreachable()` queries against
  the solver's object-layout constraints.

## Design

Four changes were implemented and measured in sequence:

1. **Skip `unreachable()` on small constant-count loops** (`src/walker/walker.cpp`):
   When `!opts.unwind` and SCEV gives a constant `getBackedgeTakenCount(l)` of
   at most `SmallConstantLoop` (64), skip `unreachable(reached)` while
   `i <= knownBackedges`. Term folding ends normal counting loops at
   `i == knownBackedges + 1` with zero solver calls; past `knownBackedges`,
   `unreachable()` still runs so loops whose exit condition does not fold
   (such as `for (unsigned i = x; i != x + 4; ++i)`) still terminate. An
   infeasible symbolic entry guard only walks at most `knownBackedges` extra
   iterations with that same UNSAT guard, so checks, trace steps, and guarded
   memory constraints stay unaffected.
2. **Memoise `unreachable(guard)` by `guard.id()`** (`src/encode/terms.h`,
   `src/encode/terms.cpp`, `src/memory/memory.cpp`, `src/walker/walker.cpp`):
   Route all walk-time global constraints through `Terms::addConstraint()`,
   which skips constant `true` constraints and increments
   `Terms::constraintsEpoch()`. `Walk::unreachable()` keeps a permanent
   `unsatGuards` set (valid monotonically as constraints are only added during
   the walk) and a `satGuards` set cleared whenever `constraintsEpoch()`
   advances.
3. **Cancel `fromBits(toBits(v, ty), ty)`** (`src/encode/encoder.h`,
   `src/encode/encoder.cpp`): In native FP mode, `Encoder::toBits` records
   `{b.id(), ieee(ty)} -> v` in `bitsFloat`, and `Encoder::fromBits` returns
   `v` directly when the loaded bit-vector matches. Camada's own axiom for
   `mkIEEEFPToBV` asserts `to_fp(__CAMADA_ieeebv_fn0(v)) = v`, so the
   round-trip is an exact identity for all IEEE-754 values, including `+0.0`,
   `-0.0`, infinities, and NaNs.
4. **Constant-bound float loop in `models/printf.c`**: Replace the nondet-bound
   loop `for (unsigned i = 0; i < k; ++i)` with `for (unsigned i = 0; i < most; ++i)`
   guarded by `if (len < cap)` and `if ((i < k) & (i < avail))`, storing at
   `s[len + i]` and advancing `len += k` once after the loop. Both the range
   clamping of `k` (`if (k < lo) k = lo; if (k > most) k = lo;`) and the
   non-NUL character (`if (!c) c = 1;`) use two-way branches without
   `__VERIFIER_assume`, so `Terms::lor` folds every diamond back to the entry
   guard and `unreachable()` hits `satGuards` on iterations `2..most-1`.

## What was not adopted

- **Initial `models/printf.c` rewrite with `__VERIFIER_assume` and `&&`/`||`**:
  Keeping `__VERIFIER_assume(c != 0)` inside the constant-bound loop polluted
  the join guard with a fresh conjunct on every iteration, and short-circuit
  `k < lo || k > most` and `i < k && i < cap - len` produced three-way CFG
  joins that `Terms::lor` does not collapse back to the entry guard. That
  defeated `satGuards` memoisation and regressed `github_9397` from 3.55 s to
  8.27 s (with `__VERIFIER_assume`) and 2.29 s (with `||`/`&&`). Using
  two-way branches and clamping without `__VERIFIER_assume` dropped walk time
  to 0.30 s.
- **Rewriting other loops in `models/*.c`**: The remaining loops in `stdio.c`
  (`readv`), `stdlib.c` (`bsearch`, `qsort`, `atoi`, `strto`), and `string.c`
  are bounded by caller arguments or string terminators rather than a fresh
  nondet variable; replacing them with worst-case constants (e.g. `UIO_MAXIOV`
  = 1024) would hurt the common constant-size case.
- **Cancelling `toBits(fromBits(b, ty), ty)`**: Not adopted, because native FP
  theory collapses all NaN payloads to a single NaN value, whereas `b` carries
  a specific bit pattern.
- **Uncapped `SmallConstantLoop`**: Kept at 64 so a large dead loop under an
  UNSAT symbolic guard still prunes on iteration 1 via `unreachable()`.

## Measurements

Incremental timings on `esbmc/github_302` and `esbmc/github_9397` (single runs
with clang 23.1.6, RelWithDebInfo):

| stage | github_302 walk / solve / wall (s) | github_9397 walk / solve / wall (s) |
|---|---|---|
| baseline (main `2263b0d3`) | 36.54 / 7.21 / 43.88 | 3.43 / 0.01 / 3.55 |
| + skip small constant loops | 0.00 / 28.98 / 29.11 | 3.43 / 0.01 / 3.57 |
| + memoise `unreachable()` | 0.00 / 28.39 / 28.52 | 3.23 / 0.01 / 3.35 |
| + cancel `fromBits(toBits)` | 0.00 / 24.10 / 24.24 | 3.40 / 0.01 / 3.53 |
| + `models/printf.c` constant loop | 0.00 / 24.32 / 24.44 | 0.29 / 0.01 / 0.42 |

Side-by-side comparison with `tests/bench.py` against main (`2263b0d3`):

| test | walk s | solve s | wall s | peak MB | verdict | change |
|---|---|---|---|---|---|---|
| esbmc/github_302 | 36.42 → 0.00 | 6.96 → 24.32 | 43.53 → 24.44 | 566 → 534 | FAILED | wall time x0.56 |
| esbmc/github_9397 | 3.43 → 0.29 | 0.01 → 0.01 | 3.56 → 0.42 | 77 → 72 | FAILED | wall time x0.12 |
| esbmc/github_9397_fail | 0.04 → 0.01 | 0.00 → 0.00 | 0.15 → 0.13 | 44 → 41 | FAILED | |
| vereda/lib_sprintf | 0.23 → 0.19 | 0.40 → 0.42 | 0.74 → 0.73 | 110 → 110 | SUCCESSFUL | |
| vereda/lib_sprintf_false | 0.06 → 0.06 | 0.05 → 0.05 | 0.22 → 0.22 | 49 → 47 | FAILED | |
| vereda/lib_sprintf_nondet_s | 0.02 → 0.02 | 0.01 → 0.01 | 0.14 → 0.13 | 43 → 42 | SUCCESSFUL | |
| vereda/variadic_vsprintf | 0.00 → 0.00 | 0.00 → 0.00 | 0.12 → 0.12 | 39 → 37 | SUCCESSFUL | |
| vereda/float_arith | 0.00 → 0.00 | 0.04 → 0.03 | 0.14 → 0.13 | 42 → 41 | SUCCESSFUL | |
| vereda/float_false | 0.00 → 0.00 | 0.01 → 0.01 | 0.11 → 0.11 | 40 → 39 | FAILED | |
| vereda/float_fold_arith | 0.00 → 0.00 | 0.00 → 0.00 | 0.12 → 0.12 | 36 → 36 | SUCCESSFUL | |
| vereda/float_intrinsics | 0.00 → 0.00 | 0.02 → 0.02 | 0.13 → 0.13 | 40 → 39 | SUCCESSFUL | |
| vereda/float_memory | 0.00 → 0.00 | 0.01 → 0.01 | 0.11 → 0.11 | 42 → 40 | SUCCESSFUL | |
| vereda/float_nan | 0.00 → 0.00 | 0.01 → 0.01 | 0.13 → 0.13 | 40 → 39 | SUCCESSFUL | |
| vereda/float_nan_payload | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 38 → 36 | SUCCESSFUL | |
| vereda/float_nan_payload_native | 0.00 → 0.00 | 0.01 → 0.01 | 0.11 → 0.11 | 40 → 40 | FAILED | |
| vereda/loop_const | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 36 → 35 | SUCCESSFUL | |
| vereda/assume_constant_loop | 0.00 → 0.00 | 0.00 → 0.00 | 0.11 → 0.11 | 37 → 36 | SUCCESSFUL | |
| vereda/assume_range_loop | 0.00 → 0.00 | 0.01 → 0.01 | 0.12 → 0.12 | 41 → 40 | SUCCESSFUL | |
| vereda/assume_range_loop_fail | 0.00 → 0.00 | 0.03 → 0.04 | 0.13 → 0.12 | 41 → 40 | FAILED | |
| vereda/summary_off | 0.00 → 0.00 | 0.00 → 0.00 | 0.10 → 0.11 | 36 → 36 | SUCCESSFUL | |

Full `ctest -j12` suite: 6892 tests (2700 passed, 4192 skipped, 0 failed), zero
regressions, and no KNOWNBUG or FUTURE test changed verdict.
