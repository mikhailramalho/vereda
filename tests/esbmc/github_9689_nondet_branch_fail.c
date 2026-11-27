// #9689: a branch on a nondet value keeps its branching waypoint.
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = 0;
  if (x > 5)
    y = 1;
  __VERIFIER_assert(y == 0); // reached
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9689_nondet_branch_fail
// Needs: --witness-output-yaml
// RUN: --witness-output-yaml -
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: type: "branching"
// EXPECT: type: "target"
// Vereda: verdict none
