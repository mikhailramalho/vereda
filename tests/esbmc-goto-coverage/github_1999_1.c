#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

bool func() { return false; }

int main()
{
  bool a = __VERIFIER_nondet_bool();
  bool b = true;
  if (b && func() || a && b)
  {
    assert(1);
  }
  else
    assert(0);
}
// Imported from ESBMC regression/goto-coverage/github_1999_1
// Needs: --condition-coverage
// RUN: --condition-coverage --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  10
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Total Conditions:  10
// EXPECT: ^Condition Properties - SATISFIED:  6
// EXPECT: ^Condition Properties - UNSATISFIED:  4
// EXPECT: ^Condition Coverage: 60%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
