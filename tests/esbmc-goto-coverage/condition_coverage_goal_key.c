#include <assert.h>
#include <stdbool.h>

int main()
{
  int a = 1;
  int b = 2;
  if (a == 1 || a == 2 || (b != 2 && (b == 3 || a == 1)))
  {
    assert(1);
  }
}
// Imported from ESBMC regression/goto-coverage/condition_coverage_goal_key
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --no-incremental-bmc
// MODE: FUTURE
// EXPECT: REACHED: 'a == 1 at file \S*condition_coverage_goal_key\.c line 8 column 3 function main'$
// EXPECT: ^Total Conditions:  8$
// Vereda: verdict none
