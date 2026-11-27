#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
int main()
{
    int a = 1;
    int b = 2;
    if ((a == 1 && b != 2) || (a == b && true))
    {
        ESBMC_assert(1);
    }
    else if ((a == 2 && b == 1) || (a == b && true))
        ESBMC_assert(0);
    else if ((a != b) || (a == b && true))
        ;
}
// Imported from ESBMC regression/goto-coverage/github_1720_23
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  12
// EXPECT: ^Short Circuited Conditions:  10
// EXPECT: ^Total Conditions:  22
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
