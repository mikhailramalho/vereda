#include <assert.h>
#include <stdbool.h>

int main()
{
  int a = 1;
  int b = 2;
  if (a == 1 || a == 2 || (b != 2 && (b == 3 || a == 1)))
  {
    assert(1);
  }
}
// Imported from ESBMC regression/goto-coverage/github_1720_5
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Condition Coverage: 12.5%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
