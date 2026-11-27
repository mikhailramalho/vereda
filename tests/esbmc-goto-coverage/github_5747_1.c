#include <assert.h>

int main()
{
  int a = 2;
  __VERIFIER_assert(!(a % 2)); // a stays even
  assert(a == 2);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_5747_1
// Needs: --assertion-coverage-claims
// RUN: --assertion-coverage-claims --no-incremental-bmc
// MODE: FUTURE
// EXPECT: Reached Assertion Instances: 2
// EXPECT: Assertion Instances Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
