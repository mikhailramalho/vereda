#include <assert.h>

int main()
{
  int *p;
  if(*p == 10)
  {
    ;
  }
}
// Imported from ESBMC regression/goto-coverage/github_1999_3
// Needs: --condition-coverage-claims --verbosity coverage:10
// RUN: --condition-coverage-claims --verbosity coverage:10
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  2
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Total Conditions:  2
// EXPECT: ^Condition Properties - SATISFIED:  2
// EXPECT: ^Condition Properties - UNSATISFIED:  0
// EXPECT: ^Condition Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
