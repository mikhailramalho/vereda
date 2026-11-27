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
// Imported from ESBMC regression/goto-coverage/github_1720_8
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts
// MODE: FUTURE
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
