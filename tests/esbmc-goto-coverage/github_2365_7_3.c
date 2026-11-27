#include <assert.h>
#include <stdbool.h>

int b;
int main()
{
  int a = __VERIFIER_nondet_int();
  //  if (a == b && b == 2)  { // |1|
  //    if ((int)(a == b && b == 2)) { // |2|
  if ((_Bool)(int)(a == b && b == 2))
  { // |3|
    assert(0 && "if");
  }
  else
    assert(0 && "else");
}
// Imported from ESBMC regression/goto-coverage/github_2365_7_3
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
