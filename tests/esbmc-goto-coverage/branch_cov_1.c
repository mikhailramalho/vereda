#include <assert.h>

int main()
{

    int f = 2;
    if (f == 2)
        f = 1;

    f = 3;
}
// Imported from ESBMC regression/goto-coverage/branch_cov_1
// Needs: --branch-function-coverage
// RUN: --branch-function-coverage --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^Function Entry Points & Branches : 3
// EXPECT: ^Reached : 2
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
