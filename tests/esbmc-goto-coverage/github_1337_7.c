#include <assert.h>

int main()
{
  for(int i = 0; i< 10; i++)
  {
    assert(0);
  }
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_1337_7
// Needs: --goto-unwind --assertion-coverage
// RUN: --goto-unwind --assertion-coverage
// MODE: FUTURE
// EXPECT: ^Total Asserts: 10
// EXPECT: ^Total Assertion Instances: 10
// EXPECT: ^Assertion Instances Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
