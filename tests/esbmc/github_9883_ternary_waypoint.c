// #9883: a ternary branching waypoint points at its ?, parenthesised or not.
extern int __VERIFIER_nondet_int(void);
int f(void) { return 2; }

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = (x > 5)
    ? f()
    : 0;
  int z = ((x > 7)
    ? f()
    : 0);
  __VERIFIER_assert(y + z == 0); // reached
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9883_ternary_waypoint
// Needs: --witness-output-yaml -
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "branching"
// EXPECT: (?s)line: 9\s+column: 5
// EXPECT: (?s)line: 12\s+column: 5
// Vereda: verdict none
