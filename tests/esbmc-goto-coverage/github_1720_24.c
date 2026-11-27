#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
int main()
{
    int b;
    if (b)
        ;
    return 0;
    int a;
    if (a + b)
        ;
}
// Imported from ESBMC regression/goto-coverage/github_1720_24
// Needs: --condition-coverage-rm
// RUN: --condition-coverage-rm
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  2
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Total Conditions:  2
// EXPECT: ^Condition Properties - SATISFIED:  2
// EXPECT: ^Condition Properties - UNSATISFIED:  0
// EXPECT: ^Condition Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
