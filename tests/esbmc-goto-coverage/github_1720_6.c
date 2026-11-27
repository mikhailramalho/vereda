#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

bool func() { return notdet_bool(); }

int main()
{
  bool a = __VERIFIER_nondet_bool();
  bool b = true;

  if (b && (a || func()) && a+b)
  {
    assert(1);
  }
  else
    assert(0);
}
// Imported from ESBMC regression/goto-coverage/github_1720_6
// Needs: --condition-coverage-claims --no-cov-asserts
// RUN: --condition-coverage-claims --no-cov-asserts --no-incremental-bmc
// MODE: KNOWNBUG
// EXPECT: ^Condition Coverage: 100%
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
