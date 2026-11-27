#include <assert.h>
#include <stdbool.h>

int b;
int main()
{
  int a = __VERIFIER_nondet_int();
  if (a == b && b == 2)
  { // |1|
    //      if ((int)(a == b && b == 2)) { // |2|
    //  if ((_Bool)(int)(a == b && b == 2)) { // |3|
    assert(0 && "if");
  }
  else
    assert(0 && "else");
}
// Imported from ESBMC regression/goto-coverage/github_2365_7_1
// Needs: --condition-coverage-claims
// RUN: --condition-coverage-claims
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  6$
// EXPECT: ^Short Circuited Conditions:  6$
// EXPECT: ^Total Conditions:  12$
// EXPECT: ^Condition Properties - SATISFIED:  4$
// EXPECT: ^Condition Properties - UNSATISFIED:  2$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
