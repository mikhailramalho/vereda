#include <stdbool.h>
#include <assert.h>

int main()
{
  bool a = true;
  bool b = false;

  assert(a != 0);
  if (a ? b ? 1 : 0 : a == b)
    ;
}
// Imported from ESBMC regression/goto-coverage/github_1720_20
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  6
// EXPECT: ^Short Circuited Conditions:  2
// EXPECT: ^Total Conditions:  8
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
