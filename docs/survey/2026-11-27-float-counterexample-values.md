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
reason. Float, double and x86 long double are printed, a long double with an `L` suffix and its 80 bits in the comment; half and fp128 assignments stay out, as do pointers, which #311 also lists. A long double goes through the host's `long double` and `printf`, since APFloat's own `toString` truncates the bits before it rounds to the digits asked for and gets the last digit wrong now and then.
