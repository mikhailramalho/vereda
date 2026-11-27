#include <assert.h>
#include <stdbool.h>

int main()
{
  int a = 1;
  int b = 2;
  if (a == 1 && (b == 2 || a == b) && 1.1)
  {
    assert(1);
  }
  else
    assert(0);
}
// Imported from ESBMC regression/goto-coverage/github_1720_13
// Needs: --condition-coverage
// RUN: --condition-coverage
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  8
// EXPECT: ^Short Circuited Conditions:  4
// EXPECT: ^Total Conditions:  12
// EXPECT: ^Condition Properties - SATISFIED:  4
// EXPECT: ^Condition Properties - UNSATISFIED:  4
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
