#include "lib.h"
int main()
{
    calculate_output();
}
// Imported from ESBMC regression/goto-coverage/github_1720_19
// Needs: --condition-coverage --no-cov-asserts -I . --include-file
// RUN: --condition-coverage --no-cov-asserts -I . --include-file lib.c --no-incremental-bmc
// FILES: lib.c
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  6
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Total Conditions:  6
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
