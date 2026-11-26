# Clang's sanitizer checks

Recorded 2026-11-23.

Vereda encodes no undefined-behaviour check of its own; the walker already
took a clang `-fsanitize=... -fsanitize-trap=...` check, `llvm.ubsantrap`,
as a failed assertion. Each check was probed with a buggy and a guarded
program: signed and unsigned overflow, shift exponent and base, division
by zero, array bounds, float-to-int overflow, null, VLA bound, pointer
overflow, alignment, implicit truncation and bool loads all give FAILED on
the bug, with the right message, and SUCCESSFUL on the guard. Nothing to
replace, then; the question is which checks to turn on.

- On request: `vereda.py --overflow-check`, `--unsigned-overflow-check` and
  `--ub-shift-check` take ESBMC's names and compile in
  signed-integer-overflow, unsigned-integer-overflow and shift. The
  importer translates them, so the 282 imported tests that were FUTURE only
  for want of one of them now run: 129 pass (116 CORE, 6 THOROUGH, 6
  FUTURE and 1 KNOWNBUG that pass outright), the rest stay FUTURE or
  KNOWNBUG with a "// Vereda:" reason. 16 more spell the flag
  `--overflow`, a prefix ESBMC's option parser accepts; the importer
  renames it, and 3 of them pass, while 12 still need other flags.
  Most of the failures are other gaps:
  missing models (rand, atoi, strerror, asprintf), variadic functions,
  llvm.ctpop, ESBMC's typecast and floating-point overflow checks and its
  message texts. One is a disagreement: github_9436 shifts `0xffff << 16`
  after promotion to int, undefined in C11 6.5.7p4, which clang flags and
  ESBMC does not. The overflow messages now use ESBMC's words ("arithmetic
  overflow on add", not "on +").
- By default, as ESBMC checks division by zero and array bounds unless
  told not to: measured over all 6612 tests, without switching. Division
  by zero puts a check in 368 tests; 4 FUTURE tests pass, 1 CORE test
  fails because Vereda has no rand model, so rand() + 1 can be 0, and 1
  times out. Array bounds puts a check in 1381: 10 FUTURE tests pass, 9
  CORE tests fail and 3 KNOWNBUG tests time out. Among the failures, one
  needs `--no-bounds-check` translated; two of those read form a pointer
  outside its array without dereferencing it, `(a-1)[2]` or `arr + 100`, undefined
  in C (6.5.6p8), which clang flags and ESBMC and CBMC accept; and in
  array_loop_summary_unroll Vereda walks the loop body under the imported
  `#pragma unroll 3` with j = 3 and writes a[3], one iteration more than
  ESBMC's `__ESBMC_unroll(2)`. Neither is on by
  default: the gain is small, and a default needs `--no-div-by-zero-check`
  and `--no-bounds-check` to turn it off again, which the importer now
  drops as checks Vereda lacks.
- SV-COMP no-overflow takes signed-integer-overflow only (see the property
  files section above).

Follow-up issues: #298.
