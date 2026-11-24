# Branching waypoints in violation witnesses

Recorded 2026-11-24. Issue #14, item 4 (branching; assumption waypoints are
left for later).

Format 2.0 puts a `branching` waypoint at the first character of an
iteration or selection statement (or the `?` of a conditional operator),
with the constraint `true` or `false` for the value the controlling
expression took, and no `format`. The trace had no branches, so the walker
now records a `Branch` step for each successor of a conditional branch that
decides a source condition: the path condition with that successor taken,
the place to look for the statement, and the value.

## Which branch decides which condition

The pass list keeps one branch per condition, but not its direction:
InstCombine swaps the successors of a branch whose condition it negates
(`x != 5` becomes `br (x == 5), else, then`), and clang already swaps them
for `!`. Branch weights would follow the swap, but clang's own successor 0
is the operand's truth, not the statement's. The names clang gives the
blocks do tell: `if.then`, `while.body`, `for.body` for true, `if.else`,
`if.end`, `while.end`, `for.end` for false. InstCombine keeps them, and
loop simplification only adds suffixes (`for.end.loopexit`). clang drops
names by default, so `vereda.py` now passes `-fno-discard-value-names`.

The names also settle `&&` and `||`. clang branches on each operand of an
`if` condition; a branch whose successor is the next operand
(`land.lhs.true`, `lor.lhs.false`) decides nothing and gets no step, and
one that jumps to `if.then` or `if.else` decides the whole condition, `!`
included. Loop conditions with `&&` are computed as a value first, so a
loop has one branch. A `&&` outside a condition (`int w = a && b;`) and the
operator `?:` (`cond.true`) get none.

## Location

For `while` and `for`, clang puts the branch at the keyword. For `if`, the
branch sits on the condition (`&&` operators included), but its scope is
the lexical block clang opens at the start of the condition, right after
`if (`. The walker records that point, and the witness steps back over `(`
to the `if` in the source text; if the text there is not `if (`, as in a
macro, the waypoint is left out. For `do`, the start is in the loop's
metadata, but CPAchecker 4.3 refutes (TRUE) a witness with such a
waypoint at the `do`, the `{`, the `}` or the `while`, while UAutomizer
confirms it at the `do`; do-while gets none.

## The waypoint right before the target

On the 42 tasks below, with every waypoint, CPAchecker lost rangesum and
rangesum05: "Refinement failed: Interpolation failed in solver MATHSAT5",
UNKNOWN, not a refutation. Leaving out single lines showed the cause is the
`if (ret != ret2 || ret != ret5)` around `reach_error()`, and a small
program of the same shape does the same. That waypoint says little the
target does not, so the branching waypoint right before the target is
left out. Without it CPAchecker confirms both again, and rangesum20 too.

Also tried: leaving out the `false` of loops, as UAutomizer fails to match
the exit of an inner loop on vogal-2 (below). It gained nothing and made
CPAchecker refute 2SB.

## Validation

The 42 `.i` ReachSafety FALSE tasks of the earlier run (41 with a witness),
witness linter (sv-witnesses fcca904), CPAchecker 4.3
`--violation-witness-validation` and UAutomizer 0.3.2
(`AutomizerReachWitnessValidation.xml`), 180 s each, main (4e50e7e8)
against this change:

| | witnesses | linter | CPAchecker | UAutomizer |
|---|---|---|---|---|
| main | 41 | 41 | 31 | 26 |
| every branching waypoint | 41 | 41 | 33 | 29 |
| this change | 41 | 41 | 36 | 29 |

No task confirmed on main is lost. CPAchecker gains
test_mutex_double_unlock, test_mutex_unlock_at_exit (both from timeouts),
simple_search_value-2, bubble_sort-2 and rangesum20; UAutomizer gains
list_and_tree_cnstr-1, simple_search_value-2 and rangesum40 (from
timeouts). On the tasks confirmed both times, CPAchecker takes 271 s in all
against 362 s (28 of 31 faster) and UAutomizer 292 s against 377 s (25 of
26 faster); UAutomizer slows on string-2 only, from 25 s to 53 s.

UAutomizer answers "waypoints that could not be matched" on vogal-2, 2SB
and the two mutex tasks, where main timed out. Its matcher (see
2026-11-24-uautomizer-function-return.md) takes a branching waypoint at an
edge starting at the keyword with a condition annotation, and the large
blocks of its SV-COMP settings merge the exit of an inner loop with the
outer loop's step and condition, which then match nothing. A related case
not among these tasks: in a small program that fills an array with
`x[i] = __VERIFIER_nondet_int();` in a loop, UAutomizer refutes (TRUE) the
witness when that loop's `true` waypoints alternate with the calls'
function_return waypoints, and confirms it with either kind alone; the
loop's condition and the call fall in one of its edges, which matches one
waypoint. Leaving out each branching waypoint followed by a function_return
does not help: on the five tasks with such a pair it fixed no UAutomizer
result and made CPAchecker refute rangesum and
verisec_OpenSER_cases1_stripFullBoth_arr, so a validator can read the
remaining waypoints differently when one goes.

Left for later: assumption waypoints; `?:`, whose waypoint goes at the `?`
while clang puts the branch at the condition; `switch`, whose constraint
is a case value (UAutomizer 0.3.2 rejects those outright); do-while, until
the validators agree.
