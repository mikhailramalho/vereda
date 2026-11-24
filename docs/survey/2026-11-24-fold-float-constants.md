# Folding floating-point operations on constants

Recorded 2026-11-24.

Issue #151, found while measuring #115 (`2026-11-24-slice-before-walk.md`):
the encoder built every floating-point operation with the solver, so a float
the program computes from constants alone, and every loop guard that tests
one, went to Bitwuzla. Without `--unwind` the walker asks the solver once per
iteration whether the next one is reachable, and on 12_lms_new and
13_ludcmp_new those calls were nearly all of the walk.

The fold lives in the encoder (`src/encode/encoder.cpp`), not in `Terms`:
`Terms` folds bit-vectors and Booleans, and a float there is either a term of
the solver's theory (natively) or a bit-vector that `Terms` already folds
(`--fp2bv`). The encoder keeps one theory term per constant float, keyed by
format and bits, with the bits of each; `fromBits` of a constant bit-vector
gives that term and `toBits` of it gives the bits back, so constants also
pass through memory. When every operand of an operation is such a constant,
the encoder computes the result with `llvm::APFloat` under
round-to-nearest-even, the only mode it encodes, and builds the constant:
fadd, fsub, fmul, fdiv, frem (`APFloat::mod`, exact; frem on other values
stays unsupported, since SMT's `fp.rem` rounds), fneg, fma and fmuladd
(fused, as encoded), sqrt (the host's `sqrt`/`sqrtf`, correctly rounded;
APFloat has none), fcmp (`FCmpInst::compare`), the truth of a float,
conversions to and from integers and between widths, fabs, copysign,
minnum/maxnum (the encoding's choice of operand) and is.fpclass. libm
functions that are not correctly rounded (sin, exp, log) are calls, never
folded.

The traps the issue lists, and the test for each under `tests/vereda`:

- Rounding mode: `fesetround` has no body, so the encoder rounds to nearest
  even whatever the program sets, and the fold does the same; the folded and
  the solver's results agree after `fesetround(FE_UPWARD)`
  (`float_fold_rounding.c`).
- -0.0 and +0.0: constants are keyed by their bits, so they are never one
  term; 1/x, copysign, signbit, fmin of equal zeros and a join of the two
  keep the sign (`float_fold_signed_zero.c`, `float_fold_signed_zero_false.c`).
- NaN: a result that is a NaN never folds, so its bits stay the encoding's,
  any NaN natively (`float_fold_nan_payload_native.c` fails the payload
  check) and the hardware's with `--fp2bv` (`float_fold_nan_payload.c`);
  `toBits` of a NaN constant is not folded either. Comparisons and class
  tests of a NaN constant fold as the encoding has them; natively both NaN
  classes of is.fpclass mean any NaN, as in `fpClass` (`float_fold_nan.c`).
- Subnormals, overflow to infinity, ties to even down to zero, and
  conversions from integers past 2^24 and 2^53 (`float_fold_subnormal.c`).
- x86_fp80: held as bits, and its arithmetic is unsupported; the fold leaves
  it alone, so a long double verdict does not depend on whether its operands
  are constants (`float_fold_long_double.c`).
- Conversions to integers: they fold only when APFloat reports the
  truncated value in range; out of it (or NaN, infinite) the conversion stays
  the solver's, not APFloat's saturated value (`float_fold_to_int.c`,
  `float_fold_to_int_range.c`).
- `float_fold_arith.c` checks that a float loop on constants unrolls without
  `--unwind`, fma and a contracted `a * b + c` round once, and square roots;
  `float_fold_frem.ll` checks frem against the host's fmod.

The fold was also checked against the unfolded encoding: a program computes
each operation on constants (folded) and on the same values the solver only
knows through `__VERIFIER_assume(bits == constant)` (not folded), and asserts
that the results have the same bits, or are both NaN. Over 13 doubles (±0,
-1/3, 0.1, the least subnormal, the least normal negated, the largest
finite, 1 + 2^-52, 3e9, -2^63, ±infinity, NaN) for fneg, fabs, sqrt,
fptrunc, the truth and class tests, fptosi and fptoui in range, and over all
pairs for fadd, fsub, fmul, fdiv, fma, copysign, fmin, fmax and the
comparisons, it passed natively and with `--fp2bv`, and so did a float run
over 12 values. The same harness with fadd folded toward zero instead failed
at the fadd assertion. It is too slow for the regression suite (14 s on its
smallest useful version, minutes natively), so it is not committed.

## Measurements

Release build, LLVM 21, `tests/bench.py --list` with main at 8333f006 as base
and this change as head, side by side:

| test | walk s | solve s | wall s | peak MB | verdict |
|---|---|---|---|---|---|
| esbmc/12_lms_new (--overflow-check) | 132.21 → 0.14 | 0.00 → 0.00 | 133.13 → 0.29 | 346 → 43 | SUCCESSFUL |
| esbmc/13_ludcmp_new | 61.51 → 0.03 | 0.00 → 0.00 | 62.14 → 0.16 | 332 → 38 | SUCCESSFUL |

A second run gave 12_lms_new 120.41 → 0.14 s of walk. Every loop guard of
the two programs now folds, so the walker unrolls each loop to its exit
without a solver call, and every overflow check of 12_lms_new folds away
too (its walk ends with 0 checks).

The float suites (the 317 tests whose name matches `float|fp|math`, run
with `REGRESS_STATS` for both binaries) give the same verdicts except
`float_fold_frem.ll`, new here (base: unsupported frem). Their tests are
small; their total wall time went from 76.3 s to 75.3 s, walk from 1.7 s to
0.8 s, solve from 6.4 s to 6.0 s.
