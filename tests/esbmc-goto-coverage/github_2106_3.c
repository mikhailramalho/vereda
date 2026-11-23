#include <stdio.h>
#include <assert.h>

int foo(int a, int b)
{
  if(a>0)
    printf("a>0\n");
  else if(b == 2)
    printf("b == 2\n");
  return a + b;
}

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  assert(foo(x,y) == 10);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_2106_3
// Needs: --condition-coverage --k-induction --base-k-step 3 --no-standard-checks
// RUN: --condition-coverage --k-induction --base-k-step 3 --function foo --no-standard-checks
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  4
// EXPECT: ^Short Circuited Conditions:  0
// EXPECT: ^Total Conditions:  4
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
