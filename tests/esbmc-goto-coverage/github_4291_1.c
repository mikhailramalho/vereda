#include <stdbool.h>

void __VERIFIER_assume(int cond);
int __VERIFIER_nondet_int(void);

bool foo(int x)
{
  if (x > 0 && x < 100)
  {
    return true;
  }
  return false;
}

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= -1000);
  __VERIFIER_assume(x <= 1000);
  foo(x);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_4291_1
// Needs: --condition-coverage-claims
// RUN: --condition-coverage-claims --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  4$
// EXPECT: ^Short Circuited Conditions:  0$
// EXPECT: ^Total Conditions:  4$
// EXPECT: ^Condition Properties - SATISFIED:  4$
// EXPECT: ^Condition Properties - UNSATISFIED:  0$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
