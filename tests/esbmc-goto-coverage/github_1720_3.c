#include <assert.h>
#include <stdbool.h>

int main()
{
  int a = 1;
  int b = 2;
  if ((a == 1 && b == 3) || (a != b && true))
  {
    assert(1);
  }
  else
    assert(0);
}
// Imported from ESBMC regression/goto-coverage/github_1720_3
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Condition Coverage: 50%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
