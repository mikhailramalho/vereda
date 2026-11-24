# Witness waypoints for pointers, floats, wide and unknown-sign values

Recorded 2026-11-24. Issue #14, item 3.

Format 2.0 (sv-witnesses, the 2.0 violation schema's description of
`function_return`) allows only `\result <op> <const_expression>`, `<op>`
one of `==`, `!=`, `<=`, `<`, `>`, `>=`, as `acsl_expression`. The guide
(2.4.1-dev) keeps that format for 2.0 compatibility and evaluates it with
C semantics as `ext_c_expression`. Each new kind of value gets what that
form can say, and nothing where it can say nothing true:

- A pointer is an object and an offset, and no constant names an object.
  A null pointer gets `\result == 0`, any other `\result != 0`. The weaker
  constraint keeps a waypoint at the call, so the waypoints of later calls
  at the same site do not bind to it (#76).
- A float or double gets a hexadecimal constant (`0x1.8p0`), exact under
  both ACSL's real and C's floating semantics, where a decimal one is
  rounded. A positive infinity gets `\result >` the largest finite value
  (`0x1.fffffffffffffp1023` for a double), a negative one `\result <` its
  negation. A NaN equals no constant and gets
  none. The sign of a zero does not show, as `-0.0 == 0.0`. The walker now
  records every direct nondet call, and the witness reads the value's type
  from the callee's declaration.
- An `__int128` comes back from x86-64 clang as `{ i64, i64 }`, packed with
  the first field lowest, which is the integer's own bit order. A value
  that fits 64 bits gets the usual constant. A wider one gets none: C has
  no wider constant, ACSL's unbounded integer constants are not C, and an
  expression such as `((unsigned __int128)hi << 64 | lo)` is not ACSL.
- A function whose name does not tell its sign (`pthread_t`, `sector_t`,
  `uint8_t`): a narrow result's sign comes from the `signext`/`zeroext` the
  ABI puts on the declaration. Otherwise a value whose top bit is clear
  reads the same in either sign and gets its constant; one whose top bit is
  set gets none. Under C semantics `\result == -1` would match an
  `unsigned long` of all ones too, but under ACSL's it would not.

## Validation

The witness linter (sv-witnesses at fcca904, `--strictChecking --svcomp`)
accepts a witness with every kind. CPAchecker 4.3
(`--violation-witness-validation`) confirms it, and rejects it (TRUE)
when any one constraint is changed to a wrong value: float, double, -inf,
null, int128 and the unknown-sign ones, so it reads each constraint.

On `nla-digbench-scaling/freire2_valuebound1.c` (ILP32, a double), the
witness CPAchecker gets is rejected, because Vereda's counterexample needs a
fused multiply-add: clang's default `-ffp-contract=on` turns `a*b + c` into
`llvm.fmuladd`, which Vereda encodes fused, and the value fails only on
hardware that fuses (checked natively with `-mfma`). Compiled with
`-ffp-contract=off`, Vereda gives another value and CPAchecker confirms that
witness, and rejects it with the value changed. Whether `vereda.py` should
pass `-ffp-contract=off` to match the validators is left open.

Ultimate Automizer 0.3.2 matches none of Vereda's `function_return`
waypoints, the integer ones from before this change included ("waypoints
that could not be matched"), while it validates sv-witnesses' example
witness and Vereda's witness with only the target. That belongs to item 2
of #14.
