# Float values in counterexamples

Recorded 2026-11-27.

A float or double assignment in the text counterexample now shows its
value as ESBMC prints it, written as a C constant as integers are since
#78: printf's `%.6e` with the exponent unpadded (`2.500000e+0`,
`2.500000e+300`), an `f` suffix for a float (`-1.000000e-1f`), and
`+INFINITY`, `-INFINITY`, `+NAN` or `-NAN` by the sign bit (`<math.h>`
macros). Six digits do not always give the value back, so the bits follow
in the comment, as for integers. ESBMC's format was kept so that its tests'
expectations, rewritten by `tests/import_esbmc.py`, keep matching:
`esbmc-floats/github_9350` now passes as imported. A hexadecimal constant
(`0x1.4p+1`, exact, as the witness writes them) was not adopted for that
reason. Half, float, double, x86 long double and fp128 are printed, with
an `L` suffix for a long double, `Q` for an fp128 and none for a half,
and all the bits in the comment. `Q` is GCC's and clang's suffix for
`__float128` and reads back exactly, where `L` would round to x86's long
double (ESBMC, whose long double is binary128 and is also its
`__float128`, prints `l` for both); C23's `f16` for a half is in the
optional Annex H, and ESBMC prints no suffix for a half. A bfloat stays
out, since its 16 bits would read as a half, and so do pointers, which
#311 also lists. Every format converts exactly to binary128, which
glibc's `strfromf128` prints with `%.6e`: APFloat's own `toString`
truncates the bits before it rounds to the digits asked for and gets the
last digit wrong now and then, and the host's `long double`, used
before, cannot hold an fp128 subnormal below 2^-16445. Stores of these
types into named variables kept in memory are shown as integer and
double stores are; an x86 long double store writes 10 of the 12 or 16
bytes of its C type, which `ir::memberPath` now accepts as filling it.
