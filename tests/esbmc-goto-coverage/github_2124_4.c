#include <assert.h>

void sort(int *ptr, int n)
{
  assert(ptr[1] == ptr[2]);
}

int main()
{
}
// Imported from ESBMC regression/goto-coverage/github_2124_4
// Needs: --no-standard-checks --condition-coverage
// RUN: --function sort --no-standard-checks --condition-coverage --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  2
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Total Conditions:  2
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
