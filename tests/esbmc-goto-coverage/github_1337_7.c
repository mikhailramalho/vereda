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
// Needs: --assertion-coverage
// RUN: --assertion-coverage
// EXPECT: ^Total Asserts: 10
// EXPECT: ^Total Assertion Instances: 10
// EXPECT: ^Assertion Instances Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// MODE: FUTURE
// Vereda: verdict none
