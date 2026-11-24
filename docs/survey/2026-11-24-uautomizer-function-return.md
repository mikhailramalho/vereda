# Where UAutomizer matches a function_return waypoint

Recorded 2026-11-24, for #138.

Ultimate Automizer 0.3.2 (`AutomizerReachWitnessValidation.xml`, through
`Ultimate.py --validate`) rejected every witness with a `function_return`
waypoint: "The witness contains waypoints that could not be matched".
CPAchecker 4.3 confirmed the same witnesses.

UAutomizer's `YamlWitnessProductAutomaton` matches a `function_return`
against the end of an ICFG edge: same end line, and, when the waypoint has
a column, `column == endColumn - 1`. It ignores the constraint. A call to a
`__VERIFIER_nondet_` function is not a call in its ICFG but a havoc inside
the edge of the statements around it, and the SV-COMP settings make that
edge a whole straight-line sequence of statements. So the waypoint matches
only at the end of the last statement of that sequence, and each edge
matches at most one waypoint. Probes, each a nondet call followed by an
`if`:

| statement | `)` of the call | `;` | no column |
|---|---|---|---|
| `x = f();` | matched | not | matched |
| `int x = f();` (also `int i, n=f(), s=0;`) | not | matched | matched |
| `if (f() == 5)` | matched | - | matched |
| `int a = f(), b = f();` (two waypoints) | not | not | not |
| `a = f();` then `b = f();` (two waypoints) | - | - | not; only `b`'s alone matches |

CPAchecker reads the constraint at the `)` and without a column (a wrong
value makes it answer TRUE), but not at the `;`. Without a column it takes
the first call on the line: for `int x = g(1) + f();` it puts the value on
`g`'s return and refutes the witness. ESBMC 8.3 writes the `)` column
too, as Vereda did.

Decision: leave the column out unless another call comes before the
nondet call on its line (a name followed by `(` that is not `if`, `while`,
`for`, `switch`, `return` or `sizeof`), where CPAchecker needs it. The
other choices lose more: the `)` loses UAutomizer on every declaration,
the `;` loses CPAchecker's reading of the value.

Measured on the 27 `.i` ReachSafety FALSE tasks whose witnesses CPAchecker
confirmed, 180 s per validator:

| witnesses | CPAchecker | UAutomizer |
|---|---|---|
| main | 26 (rangesum20 UNKNOWN) | 22 |
| this change | 26 (rangesum20 UNKNOWN) | 23 |
| earlier witnesses, `)` column | 27 | 16 |
| earlier witnesses, this rule | 27 | 18 |

All witnesses pass the linter. Only 7 of main's witnesses have a
`function_return` waypoint; UAutomizer confirms 2 of them on main, 3 with
this change. rangesum20 has none on main and CPAchecker runs out of time
on it either way. The earlier
witnesses, written before #129 was merged, have them also for nondet
functions declared without a prototype (`extern int
__VERIFIER_nondet_int();`), which main now skips: clang calls such a
function through `i32 ()` while declaring it `i32 (...)`, so the walker's
`getCalledFunction()` is null and it records no step. That is a separate
bug.

The 9 earlier witnesses UAutomizer still rejects are the sequence limit above: two
nondet calls in one declaration (sum03-1), consecutive nondet assignments
(id_trans, divbin), `x[i] = f();` whose sequence ends at the loop's `i++`
(rangesum), and one call at the top of a `while (1)` body
(simple_array_index_value_1-2) that the line alone does not match. No location the
format allows fixes these; only fewer waypoints would.
