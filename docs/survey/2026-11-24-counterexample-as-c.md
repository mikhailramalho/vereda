# Counterexample printed as C

Recorded 2026-11-24.

Issue #78 asked for a counterexample that can be pasted into a C program:
`x = 7; // 00000000 ... 00000111` instead of ESBMC's `x = 7 (00000000 ...)`,
the state header as a `//` comment and no separator line.

The first try made every other line a `//` comment too, the Assumption and
Violated property blocks included (`//   file ...`, `//   __VERIFIER_assert`).
That broke the expectations on the lines of the violated property, which
imported ESBMC tests write as `^  dereference failure: NULL pointer$`,
`^\s*arithmetic overflow on add$` and the like, and which cannot be told
apart mechanically from other output that starts with two spaces (the
results list, `CWE:` lines, witness and GOTO dumps). Not adopted: those two
blocks are instead wrapped in a `/*` and `*/` line each, so their lines stay
as ESBMC prints them and those expectations keep matching unchanged.

A value is a C constant where C has one: the least 64-bit value is
`-9223372036854775807 - 1` (`-9223372036854775808` negates a constant with no
type), an unsigned value past `LLONG_MAX` takes a `u` suffix, and a value
wider than 64 bits, which has no C constant, or one the model lacks makes
the whole line a comment. Booleans print as `true`/`false`. Floats and
pointers are not printed today (the trace shows integer values only).

`tests/import_esbmc.py` rewrites ESBMC's counterexample expectations the same
way (`as_c`). Applying it to the tests already there rewrote the
expectations of 27 imported tests and 5 of Vereda's own
(`vereda/loop_nondet_false.c` finished by hand), and the new constant for
the least value made `esbmc/cpachecker-long-min-64.c` pass, so it went back
to THOROUGH.
