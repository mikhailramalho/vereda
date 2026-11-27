#include <assert.h>
#include <stdbool.h>

int b;
int main()
{
  int a = __VERIFIER_nondet_int();
  //    if ((a == b) + 1 && b == 2) { // |1|
  if (((a == b) + 1 && b == 2) != 0)
  { // |2|
    assert(0 && "if");
  }
  else
    assert(0 && "else");
}
// Imported from ESBMC regression/goto-coverage/github_2365_6_2
// Needs: --condition-coverage-claims
// RUN: --condition-coverage-claims
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  10
// EXPECT: ^Short Circuited Conditions:  6
// EXPECT: ^Total Conditions:  16
// EXPECT: ^Condition Properties - SATISFIED:  6
// EXPECT: ^Condition Properties - UNSATISFIED:  4
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
