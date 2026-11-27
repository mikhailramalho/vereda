#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

bool func() { return notdet_bool(); }

int main()
{
  bool a = __VERIFIER_nondet_bool();
  bool b =  true;
  bool c = __VERIFIER_nondet_bool();
  if (b && c|| a && b) 
  {
    assert(1);
  }
  else
    assert(0);
}
// Imported from ESBMC regression/goto-coverage/github_1720_7
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  6
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
