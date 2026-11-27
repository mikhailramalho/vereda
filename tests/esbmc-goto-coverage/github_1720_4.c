#include <assert.h>
#include <stdbool.h>

int main()
{
  int a = 1;
  int b = 2;
  while(a && (!b && !a || !b) && true)
  {
    assert(1);
  }
}
// Imported from ESBMC regression/goto-coverage/github_1720_4
// Needs: --condition-coverage --no-cov-asserts
// RUN: --condition-coverage --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  4
// EXPECT: ^Short Circuited Conditions:  2
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
