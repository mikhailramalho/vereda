#include <assert.h>
#include <stdlib.h>

void calculate_output()
{
    int input = __VERIFIER_nondet_int();
    if (input == 2)
        assert(1);
    if (input == 1)
       ;
}

int main()
{
    calculate_output();
}
// Imported from ESBMC regression/goto-coverage/github_1720_12
// Needs: --condition-coverage-claims --no-cov-asserts --goto-unwind
// RUN: --condition-coverage-claims --no-cov-asserts --goto-unwind
// MODE: FUTURE
// EXPECT: ^Condition Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
