#include <stdbool.h>

int main()
{
  int a = 2;
  bool b[10];
  if ((double)a || b && a)
    ;
}
// Imported from ESBMC regression/goto-coverage/github_1720_14
// Needs: --condition-coverage --no-cov-asserts
// RUN: --condition-coverage --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  2
// EXPECT: ^Short Circuited Conditions:  4
// EXPECT: ^Total Conditions:  6
// EXPECT: ^Condition Properties - SATISFIED:  1
// EXPECT: ^Condition Properties - UNSATISFIED:  1
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
