#include <assert.h>
#include <stdbool.h>

int a;
int b;

struct foo
{
  _Bool x;
};

int main()
{
  int a = __VERIFIER_nondet_int();
  struct foo *npe = 0;
  __VERIFIER_assume(a != 1);
  if (a == 1)
  {
    if (npe->x)
    {
      assert(0 && "if");
    }
    else
    {
      assert(1 && "else");
    }
  }
  else
    assert(1 && "else");
}
// Imported from ESBMC regression/goto-coverage/github_2365_3
// Needs: --condition-coverage-claims
// RUN: --condition-coverage-claims
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  6$
// EXPECT: ^Short Circuited Conditions:  10$
// EXPECT: ^Total Conditions:  16$
// EXPECT: ^Condition Properties - SATISFIED:  3$
// EXPECT: ^Condition Properties - UNSATISFIED:  3$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
