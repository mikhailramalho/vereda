# Floating point

Recorded 2026-11-22.

A float is the bit-vector of its IEEE bits everywhere: in the value map,
in memory, in phis and selects, and for nondet values, so loads, stores,
unions and int<->float bitcasts need nothing new. An FP instruction views
its operands as camada's bit-vector encoded floating point
(FPEncoding::BV, through mkBVToIEEEFP and back with mkIEEEFPToBV), which
camada bit-blasts itself with hardware NaN payloads; the native FP sort
was not used, as its single abstract NaN makes the bits of a NaN written
to memory unspecified. Rounding is to nearest, ties to even; fesetround is
not modelled. fneg, fabs, copysign and llvm.is.fpclass are bit operations
in the term builder, so they fold. fptosi and fptoui truncate (camada's
mkFPToSBV; float_arith checks it). frem stays unsupported: it is C's fmod,
which truncates, and SMT-LIB's fp.rem rounds to nearest. x86_fp80 (long
double) is not an IEEE interchange format and stays unsupported.

Later the same day the default became the solver's own floating-point
theory (FPEncoding::Native), with `--fp2bv` for camada's encoding, as in
ESBMC. Values stay bit-vectors either way; only the view inside an
operation changes. Under the solver's theory a NaN result's bits are some
NaN, as camada's mkIEEEFPToBV documents, while `--fp2bv` keeps payloads
(float_nan_payload and float_nan_payload_native pin the difference). The
float tests give the same verdicts both ways and on Z3 and cvc5; Bitwuzla
is fastest, Z3 slowest (20 s against 0.5 s on float_intrinsics). With
`--fp2bv` translated by import_esbmc.py, 10 more imported tests pass.

Keeping floats as bits under the solver's theory turned out slow: SMT-LIB
has no float-to-bits operation, and camada emulates mkIEEEFPToBV on
Bitwuzla and cvc5 with an uninterpreted function tied by
`to_fp(fn(x)) == x`, so every FP result added a function application and
a constraint. 10_fir_new (`--unwind 34`) took 0.48 s to walk and did not
finish solving in 10 minutes. In native mode a float is now an FP term,
converted to bits only at memory, bitcasts, vector elements and
copysign's sign test; fneg and fabs are FP operations, and is.fpclass
uses the theory's class tests (one NaN, so either NaN class means any
NaN). x86_fp80 stays bits, with neg and fabs as bit operations.

| Build | Walk | Solve | Peak memory |
|---|---|---|---|
| 1266dcdf (fp2bv only) | 8.6 s | 12.8 s | 3.1 GB |
| native, floats as bits | 0.48 s | over 10 min | |
| native, floats as FP terms | 0.42 s | 0.20 s | 107 MB |
| `--fp2bv`, same build | 13.6 s | 20.8 s | 3.1 GB |

The float tests keep their verdicts and run faster native than with
`--fp2bv` (float_intrinsics 0.03 s against 0.08 s). One imported test,
native_fp_nan_sign, goes back to KNOWNBUG: it reads a NaN's sign bit,
which the theory does not fix.

Clang keeps sqrt and fma as library calls while errno is on, so a pass in
ir turns calls to bodyless libm functions with an LLVM intrinsic (sqrt,
fabs, floor, ceil, trunc, round, rint, nearbyint, roundeven, fmin, fmax,
fma, copysign, and the f and l variants) into that intrinsic; errno is not
set, as in ESBMC's models.

Imported ESBMC tests: 84 now pass (83 FUTURE, 1 KNOWNBUG), none regress.
Of the unsupported reasons, floating point is gone except x86_fp80 (12
tests); the largest left are vector global initialisers (110), atomicrmw
and cmpxchg (17), vector instructions and values, and variadic functions.
The verdict check's 188 tasks give one new correct SUCCESSFUL
(bary_diverge, UNKNOWN before) and no other change. Some promoted tests
(10_fir_new, about 38 s alone) come close to the 60 s limit for imported
tests and can time out under a full parallel run. Not yet done: printing
float values in the counterexample, constant folding of FP operations in
the term builder, fmod, and the rounding mode.
