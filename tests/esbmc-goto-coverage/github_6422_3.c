int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 0)
    return 1;
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6422_3
// Needs: --branch-function-coverage
// RUN: --branch-function-coverage --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Function Entry Points & Branches : 3
// EXPECT: ^Reached : 3
// EXPECT: ^Branch Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
