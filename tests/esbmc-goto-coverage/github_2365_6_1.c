#include <assert.h>
#include <stdbool.h>

int b;
int main()
{
  int a = __VERIFIER_nondet_int();
  if ((a == b) + 1 && b == 2)
  { // |1|
    //  if (((a == b) + 1 && b == 2) != 0) { // |2|
    assert(0 && "if");
  }
  else
    assert(0 && "else");
}
// Imported from ESBMC regression/goto-coverage/github_2365_6_1
// Needs: --condition-coverage-claims
// RUN: --condition-coverage-claims --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  8$
// EXPECT: ^Short Circuited Conditions:  6$
// EXPECT: ^Total Conditions:  14$
// EXPECT: ^Condition Properties - SATISFIED:  5$
// EXPECT: ^Condition Properties - UNSATISFIED:  3$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
