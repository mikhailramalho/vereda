int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6422_2
// Needs: --condition-coverage --schedule
// RUN: --condition-coverage --schedule
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  2
// EXPECT: ^Total Conditions:  2
// EXPECT: ^Condition Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
