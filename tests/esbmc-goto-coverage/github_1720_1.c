#include <assert.h>
#include <stdbool.h>

int main()
{
  int a = 1;
  int b = 2;
  if ((a == 1 && b == 2 || a == b && true))
  {
    assert(1);
  }
  else
    assert(0);
}  
// Imported from ESBMC regression/goto-coverage/github_1720_1
// Needs: --condition-coverage --no-cov-asserts
// RUN: --condition-coverage --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Condition Coverage: 25%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
