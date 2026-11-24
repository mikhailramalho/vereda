# The waypoint of a non-null nondet pointer

Recorded 2026-11-24, for #139.

#129 gives a nondet pointer that is not null the constraint `\result != 0`,
the only true thing format 2.0 can say of it. The other choice is no
waypoint at all. Both were measured, and `\result != 0` stays.

## Tasks

No ReachSafety task calls a nondet function that returns a pointer. In the
local sv-benchmarks, the downloaded ldv sets and aws-c-common, and by GitHub
code search on a mirror, `__VERIFIER_nondet_pointer` is called only under
`todo/` directories, which no task definition lists;
`__VERIFIER_nondet_pchar` only in `ddv-machzwd` (concurrency) and
`__VERIFIER_nondet_charp` only in `heap-data/packet_filter` (expected true).
Of the four `todo/` programs expected false, Vereda answers UNKNOWN on three
(`llvm.ptrmask`, a call through an unknown target), and CPAchecker cannot
parse the fourth (imon).

So the measurement uses small programs, LP64, written to put a nondet
pointer, null or not, on the path to `reach_error`, alone and mixed with
`__VERIFIER_nondet_int` at the same and at later call sites. UAutomizer 0.3.2
does not handle `__VERIFIER_nondet_pointer` ("not defined or handled
internally") and answers TRUE on every program that calls it, even with a
witness holding only the target, so each program whose result it can change
also has a copy with `__VERIFIER_nondet_pchar` (the `c` rows).

## Results

CPAchecker 4.3 (`--violation-witness-validation`) and UAutomizer 0.3.2
(`AutomizerReachWitnessValidation.xml`), 180 s each. FALSE confirms the
witness, TRUE rejects it, unmatched is UAutomizer's "waypoints that could
not be matched". The linter (2.4.1-dev, `--svcomp`) accepts every witness.

| program | waypoints keep / drop | CPAchecker keep / drop | UAutomizer keep / drop |
|---|---|---|---|
| p01 null | 1 / 1 | FALSE / FALSE | TRUE / TRUE |
| p02 non-null | 1 / 0 | FALSE / FALSE | TRUE / TRUE |
| c02 non-null | 1 / 0 | FALSE / FALSE | FALSE / FALSE |
| p03 non-null, then an int | 2 / 1 | FALSE / FALSE | FALSE / FALSE |
| p04 null, then an int | 2 / 2 | FALSE / FALSE | FALSE / FALSE |
| p05 loop, null only in the 2nd iteration | 3 / 1 | TRUE / FALSE | TRUE / TRUE |
| c05 | 3 / 1 | TRUE / FALSE | unmatched / unmatched |
| p06 loop, non-null then an int | 6 / 3 | FALSE / TRUE | TRUE / TRUE |
| c06 | 6 / 3 | FALSE / TRUE | unmatched / FALSE |
| p07 loop, an int only when non-null | 5 / 3 | TRUE / TRUE | unmatched / unmatched |
| c07 | 5 / 3 | TRUE / TRUE | unmatched / unmatched |
| p08 helper called twice, non-null then null | 3 / 2 | FALSE / TRUE | TRUE / TRUE |
| c08 | 3 / 2 | FALSE / TRUE | FALSE / FALSE |
| p09 helper returning an int unless null, 3 calls | 5 / 3 | FALSE / FALSE | TRUE / TRUE |
| c09 | 5 / 3 | FALSE / FALSE | FALSE / FALSE |
| p10 list of ints, then a pchar | 4 / 3 | UNKNOWN / UNKNOWN | unmatched / unmatched |
| p11 loop, an int then a pointer | 6 / 4 | FALSE / TRUE | unmatched / unmatched |
| c11 | 6 / 4 | FALSE / TRUE | unmatched / unmatched |

CPAchecker confirms 13 of the 18 witnesses with `\result != 0` and 9
without; UAutomizer 5 with and 6 without. Dropping the waypoint loses
CPAchecker six programs (p06, c06, p08, c08, p11, c11) and wins it two
(p05, c05); it wins UAutomizer one (c06).

## Why

CPAchecker does not tell apart waypoints at one location that follow each
other. In p08 the helper's call site returns a non-null pointer and then a
null one; without the first waypoint, CPAchecker puts `\result == 0` on the
first call and rejects the witness, which is #76. In the loops, the pointer
waypoints sit between the integer waypoints of another line; without them,
two integer waypoints at one line come one after the other, and CPAchecker
rejects that too: a loop with only `__VERIFIER_nondet_int` (three waypoints
at one line, all correct) is rejected the same way. p05 and c05 lose with
`\result != 0` for that reason, three pointer waypoints at one line in a
row. It is not only the line: of p05's three, the pair `!= 0`, `== 0` is
rejected, while `== 0`, `!= 0` and `!= 0`, `!= 0` are confirmed.

UAutomizer ignores the constraint and loses c06 with the waypoint because
the pchar call is the first statement of the loop body, which it does not
match (as `simple_array_index_value_1-2` in the 2026-11-24 note on
`function_return`); without it only the integer waypoints are left, which it
matches.

Decision: keep `\result != 0`. `tests/vereda/witness_pointer_same_site.c`
has p08's shape and pins it; CPAchecker confirms its witness, and rejects
it without the waypoint. That integer-only witnesses in loops are rejected by
CPAchecker is a separate problem, not one a pointer constraint can fix.
