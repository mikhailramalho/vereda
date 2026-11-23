# Violation witnesses

Recorded 2026-11-23.

`--witness FILE` writes the first counterexample that reaches
`reach_error` as a violation witness in the YAML format, version 2.0 (sv-witnesses,
`format/schemas/archive/v2.0/violation-witness-2.0-schema.yml`). The
witness is one `violation_sequence` entry: a `function_return` waypoint
for each integer a `__VERIFIER_nondet_` function of known sign returned
on the path, with the constraint `\result == v`, then the `target`. This is the shape Symbiotic
writes, and the cheapest that still fixes the path: the walker already
binds a fresh term at each such call, so the trace gains a `Call` step
with that term, and the witness reads its value from the model. Branching
and assumption waypoints would need the walker to keep branch locations
and source expressions; they are left until a validator needs them.

Decisions, from the 2.0 schema and the guide (version 2.4.1-dev):

- `format_version` is "2.0", the version every validator reads. Later
  versions only add to it.
- A `function_return` waypoint points at the `)` that closes the call.
  The debug location gives the start of the callee name, so the writer
  checks that the name is there and scans the line for the matching
  parenthesis. When it is not there (a call a macro expands to is located
  at the macro's name) or the call spans lines, the column is left out,
  which the format reads as the first suitable place on the line.
- The `target` points at the statement that violates the property. For
  unreach-call that is the call to `reach_error`, not the `__assert_fail`
  inside SV-COMP's `reach_error`, so the walker keeps the call being walked
  and the trace's check step takes its location. CPAchecker and Symbiotic
  put the target of the guide's example at that same call.
- The value goes in the callee's sign, by the SV-COMP name: a bodyless
  function has no debug type. The sign matters for types narrower than
  int, which C promotes before the comparison. Only names whose sign C
  fixes get a waypoint (`int`, `long`, `uchar`, `size_t` and the like);
  `pthread_t`, `sector_t` and other typedef names, and other bodyless
  functions, get none. Pointers, floats, values wider than 64
  bits and INT64_MIN, which has no decimal constant, get no waypoint;
  a witness with fewer waypoints is still valid, only less precise.
- Waypoints point only into the input files: the compile units' main
  files, hashed with SHA-256. Calls in headers, in the library models
  (built without -g) and the nondet calls for uninitialised locals (no
  debug location) are left out. When the target is outside them there is
  no witness, and vereda says why: a validator rejects a file the task
  does not list.
- The specification is always unreach-call from `--entry`, so a witness
  is written only when the violation is a call to `reach_error`, bodyless
  or with the check inside it. A failed `assert`, `__VERIFIER_assert`,
  sanitizer check or unwinding assertion gets none. Under
  `--multi-property` the first counterexample that reaches `reach_error`
  is the one written.
- Follow-up: SV-COMP's `.i` tasks carry linemarkers, so debug locations
  name the original `.c` file while the task's input file is the `.i`.
  Today such a witness is refused, as the target is outside the input
  files. Mapping locations back to `.i` lines is the fix, and it matters
  for most SV-COMP tasks.
- Correctness witnesses are not written. They need loop invariants, and a
  bounded proof has none to give.

The witness for the guide's `unsafe-program-example.c`, with an SV-COMP
`reach_error`, passes the 2.0 JSON schema, and its columns and target match
Symbiotic's. The linter (`witnesslint`) was not run: it needs packages (libclang,
pycparser) that are not installed here.

Counterexample replay moved to a later milestone. Notes for that work:
the prepared module, not the clang output, should run under lli, so the
run follows the semantics that were checked; the nondet values can come
from the same `Call` steps, served in walk order (on one path, walk order
is execution order). The call sites, not the declarations, are the place
to substitute values, since K&R declarations are variadic. A replay
should mark the violated check's instruction (which `Check` would have to
keep) and count it reached only when its condition fails there (the
argument of `__VERIFIER_assert`, true for `reach_error`). It cannot be
faithful where the walker's model differs from libc: malloc failing,
unknown externals with side effects, `__vereda_object_size`, and pointer
values.
