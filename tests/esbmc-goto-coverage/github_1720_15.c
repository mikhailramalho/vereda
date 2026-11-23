#include <assert.h>
#include <stdbool.h>

int main()
{
  int a = 1;
  int b = 2;
  while(a && (!b && !a || b) && true)
  {
    assert(1);
  }
}
// Imported from ESBMC regression/goto-coverage/github_1720_15
// Needs: --condition-coverage --no-cov-asserts
// RUN: --condition-coverage --no-cov-asserts --unwind 10 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  6
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Condition Coverage: 50%
// EXPECT: ^COVERAGE ANALYSIS INCOMPLETE
// Vereda: verdict none
