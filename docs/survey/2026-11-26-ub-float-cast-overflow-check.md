# Out-of-range floating-point to integer conversion checks (#261)

## Design

Implements #261 via the opt-in `--ub-float-cast-overflow-check` flag, independent of
`--overflow-check`, `--no-assertions`, and SV-COMP property modes (`unreach-call` and
`no-overflow`). Violations use `checks::Property::FloatCastOverflow` (`<fn>.float-cast-overflow.<n>`)
and comment `"float cast overflow"`.

### C frontend instrumentation

When `--ub-float-cast-overflow-check` is active under `--property all` for C inputs,
`vereda.py` passes `-fsanitize=float-cast-overflow -fno-sanitize-recover=float-cast-overflow
-fsanitize-debug-trap-reasons=none`. Clang emits open-bound ordered `fcmp` checks matching
C11 6.3.1.4p1 and calls `@__ubsan_handle_float_cast_overflow_abort`. Gating on
`selected.property == "all"` (and `property == Property::All` in `main.cpp`) ensures
`unreach-call` and `no-overflow` runs never prune paths on unchecked float casts.

### Raw LLVM IR instrumentation

`checkFloatCasts` in `src/ir/load.cpp` instruments `fptosi` and `fptoui` instructions
that are not already guarded by Clang's `__ubsan_handle_float_cast_overflow_abort`,
covering raw `.ll`/`.bc` inputs, library models, and vector casts (reducing all lanes of
`FixedVectorType` before scalarization). To avoid rounded limit constants:

- **Upper bound:** `x < 2^E` (`E = W - 1` for signed, `W` for unsigned), which is an
  exact power of two whenever `E <= semanticsMaxExponent(sem)`, else `x < +inf`.
- **Lower bound:**
  - Unsigned: `x > -1.0` (exact in every IEEE format; negative fractions in `(-1, 0]`
    truncate to `0`).
  - Signed: when `W - 1 > semanticsMaxExponent(sem)`, `x > -inf`; when `W <= precision(sem)`,
    `-(2^(W-1) + 1)` is exact and checked with `x > -(2^(W-1) + 1)`; when `W > precision(sem)`,
    ULP at `-2^(W-1)` is at least 2 so no float lies in `(-2^(W-1) - 1, -2^(W-1))`, making
    `x >= -2^(W-1)` exact.

Ordered `fcmp` predicates automatically reject NaN and infinities. Constant-safe checks
are removed by `dropSafeChecks` after `mem2reg` and `InstCombine`.

## Validation

- `tests/float_cast_overflow.py` (`vereda/float_cast_overflow`) exercises C11 boundaries,
  signed/unsigned destinations, `float`/`double`/`long double` (`x86_fp80`), NaN/infinities,
  `--fp2bv`, `--incremental-solver`, `--no-assertions`, `--multi-property`, property
  filtering (`unreach-call`, `no-overflow`), raw IR widths (`i1`, `i128`), and vector lanes.
- Full `vereda` regression suite passes with zero failures.
