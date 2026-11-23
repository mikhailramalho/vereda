#include <assert.h>
#include <stdbool.h>

int main()
{
  if (1 && 2 && 3 && 4)
    ;
}
// Imported from ESBMC regression/goto-coverage/github_1720_17
// Needs: --condition-coverage --no-cov-asserts
// RUN: --condition-coverage --no-cov-asserts --no-simplify
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  8
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Total Conditions:  8
// EXPECT: ^Condition Coverage: 50%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
