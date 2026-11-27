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
// Imported from ESBMC regression/goto-coverage/github_1720_21
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  2
// EXPECT: ^Short Circuited Conditions:  2
// EXPECT: ^Total Conditions:  4
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
