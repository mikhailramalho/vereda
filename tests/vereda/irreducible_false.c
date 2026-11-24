// irreducible.c's goto into a loop, with a bug on the path that takes it.
// The block FixIrreducible adds branches to the way in taken, which no
// source condition decides: the witness has the if's waypoint and the
// while's, one per test of its condition, and none from that block.
// RUN: --witness -
// EXPECT: value: "true"(\n.*){2}\n +line: 15\n +column: 3\n[\s\S]*value: "true"(\n.*){2}\n +line: 17\n +column: 3\n[\s\S]*value: "true"(\n.*){2}\n +line: 17\n +column: 3\n[\s\S]*value: "false"(\n.*){2}\n +line: 17\n +column: 3\n
// EXPECT-NOT: (type: "branching"[\s\S]*){5}
// EXPECT: ^VERIFICATION FAILED$
extern void __assert_fail(const char *, const char *, unsigned, const char *);
void reach_error(void) { __assert_fail("0", "irreducible_false.c", 3, "reach_error"); }
extern int __VERIFIER_nondet_int(void);

int main(void) {
  int i = 0, s = 0;
  if (__VERIFIER_nondet_int())
    goto inside;
  while (i < 3) {
    s += 2;
  inside:
    i++;
  }
  if (s != 6)
    reach_error();
  return 0;
}
