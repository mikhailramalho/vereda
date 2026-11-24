// --witness gives each if, while and for condition on the path a branching
// waypoint at the statement's keyword, with the value the whole condition
// took. clang branches on each operand of && and ||; only the branch that
// decides the condition counts. InstCombine swaps the successors of the
// first if (x != 5 becomes x == 5); the names of the blocks keep the value
// right. A do-while gets none: CPAchecker 4.3 refutes such a waypoint. Nor
// does the if around reach_error, right before the target.
// RUN: --witness -
// EXPECT: value: "false"(\n.*){2}\n +line: 23\n +column: 3\n
// EXPECT: value: "true"(\n.*){2}\n +line: 25\n +column: 3\n
// EXPECT: value: "false"(\n.*){2}\n +line: 25\n +column: 3\n
// EXPECT: value: "false"(\n.*){2}\n +line: 27\n +column: 5\n
// EXPECT: value: "true"(\n.*){2}\n +line: 27\n +column: 5\n
// EXPECT: value: "false"(\n.*){2}\n +line: 31\n +column: 3\n
// EXPECT: value: "true"(\n.*){2}\n +line: 32\n +column: 3\n
// EXPECT-NOT: line: (30|33|35)\n
// EXPECT: VERIFICATION FAILED
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "witness_branching.c", 3, "reach_error"); }
extern int __VERIFIER_nondet_int(void);
int main(void) {
  int x = __VERIFIER_nondet_int(), i = 0;
  if (x != 5)
    return 0;
  while (i < 2) {
    i++;
    if (i == 2 && x > 0)
      i++;
  }
  do i--; while (i > 5);
  for (int j = 0; j < i - 2; j++) {}
  if ((x == 5) &&
      (i == 2 || x < 0))
    i = 7;
  if (i == 7)
    reach_error();
  return 0;
}
