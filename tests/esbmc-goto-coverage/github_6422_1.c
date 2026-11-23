int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6422_1
// Needs: --branch-coverage --schedule
// RUN: --branch-coverage --schedule
// MODE: FUTURE
// EXPECT: ^Coverage goals: 2 ✓ 2 reached$
// EXPECT: ^Branches : 2
// EXPECT: ^Reached : 2
// EXPECT: ^Branch Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
