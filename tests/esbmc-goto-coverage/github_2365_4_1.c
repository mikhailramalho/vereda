#include <assert.h>
#include <stdbool.h>

int b;
int main()
{
  int a = __VERIFIER_nondet_int();
  if (a == 1 && b == 2)
  { // |1|
    //  if ((a == 1 && b == 2) != 0) { // |2|
    assert(0 && "if");
  }
  else
    assert(0 && "else");
}
// Imported from ESBMC regression/goto-coverage/github_2365_4_1
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
