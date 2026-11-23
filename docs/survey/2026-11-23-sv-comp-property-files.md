# SV-COMP property files

Recorded 2026-11-23.

`vereda.py --property-file f.prp` reads the property, as SV-COMP and
benchexec pass it, and the binary's `--property` counts only that
property's failures. A failure that does not count ends its path, as the
abort it is, and the run prints ESBMC's wrapper verdict (`TRUE`,
`FALSE_REACH`, `FALSE_OVERFLOW`, `UNKNOWN`) after its own.

- unreach-call (`G ! call(reach_error())`): a call to reach_error is the
  violation and its body is not walked, as ESBMC's
  `--enable-unreachability-intrinsic` does. assert, `__assert_fail`,
  `__VERIFIER_error` and `__builtin_unreachable` are not violations; a
  failed assert assumes its
  condition. Only reach_error is accepted, the name current tasks use.
- no-overflow (`G ! overflow`): clang compiles with
  `-fsanitize=signed-integer-overflow` and `-fsanitize-trap`, and only its
  checks count. SV-COMP's overflow is a signed result out of range; clang's
  shift checks would also flag a negative left operand, so they are left
  out. An .ll or .bc input is refused: its checks would have to be
  compiled in already.
- Anything else, valid-memsafety among them, is refused with an error:
  Vereda has no memory-safety checks yet (plan section 7.2), and a
  run that ignored them would answer TRUE without looking.

A failed unwinding assertion gives `UNKNOWN`, never a FALSE. The parser
takes one CHECK line; the memory-safety file has three and is refused
anyway. run.py replaces `%S` in RUN lines with the test's directory, so
the tests can name the property files in `tests/vereda/properties/`.
