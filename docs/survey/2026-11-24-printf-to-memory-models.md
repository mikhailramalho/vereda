# Models of the printf functions that write to memory

Recorded 2026-11-24.

`models/printf.c` models sprintf, snprintf, vsprintf, vsnprintf, asprintf,
vasprintf and glibc's `__*_chk` forms (vereda.py compiles at -O0, so clang
never emits those itself; they come only from preprocessed `.i` files
built with _FORTIFY_SOURCE). One C function walks the format and reads the
arguments with va_arg, which the walker already supports in a defined
variadic function (2026-11-23-variadic-calls.md). With a constant format
the format's bytes stay known and every test on them folds, so only the
conversions' values reach the solver.

Exact, as glibc writes them in the C locale (checked natively against
glibc on 64 and 32 bits): %d %i %u %o %x %X %c %s %p %n %%, with the flags
`-+ #0`, width and precision (also `*`) and the lengths hh h l ll j z t q.
%ls and %lc narrow each wide character, which is what the C locale does
for ASCII. A floating conversion (%f %F %e %E %g %G %a %A) writes a nondet
number of nondet bytes other than NUL, from 1 (or the width) to the
longest text it can give: 309 integer digits plus point, precision and
sign for %f of a double, precision + 9 for %e, precision + 8 for %g, 13 or
16 hex digits + 12 for %a. %Lf reads a long double, which stays unsupported.

Two choices came from measurements. Digits of a value of 32 bits or fewer
are divided out as 32-bit numbers: with 64-bit division throughout,
esbmc/sprintf_symbolic_digits took 16 s, with this 0.8 s. And %s of a null
pointer calls the same string function with glibc's "(null)" rather than
choosing that pointer in place: a read through a pointer that may be one
of two objects moves every known byte, the format's included, into the
memory arrays, after which the format no longer folds and the walk reaches
the long double branch (esbmc/github_8524_null_fail went UNKNOWN on
llvm.ptrmask).

asprintf formats twice, once to size the object and once to fill it, and
assumes both give the same length, so that a floating conversion's nondet
length is the same in both.

Not done: a bounds check in the model itself (through
`__vereda_object_size`). Vereda has no valid dereference checks yet
(issue #29), so a write past the buffer goes unreported here as anywhere
else; each byte is a store to the buffer, which those checks will see.

Of the 87 imported tests that call these functions, 25 FUTURE tests now
give the expected verdict and are CORE. esbmc/vsprintf_stores_output_fail
went from UNKNOWN to a wrong SUCCESSFUL (an overflow, #29), and
esbmc/sprintf_objsize_s_no_overflow from a wrong FAILED to UNKNOWN (%s of
an uninitialised array reads past it until the bound).

Follow-up issues: #305.
