#include <assert.h>
int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x > 0)
    assert(x > 0);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6422_5
// Needs: --assertion-coverage --schedule
// RUN: --assertion-coverage --schedule
// MODE: FUTURE
// EXPECT: ^Total Asserts: 1
// EXPECT: ^Total Assertion Instances: 1
// EXPECT: ^Reached Assertion Instances: 1
// EXPECT: ^Assertion Instances Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
