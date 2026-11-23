#include <assert.h>
#include <stdbool.h>

int b = 2;
int main()
{
  int a = __VERIFIER_nondet_int();
  // if (a == 1 && b == 2) { // |1|
  if ((a == 1 && b == 2) != 0)
  { // |2|
    assert(0 && "if");
  }
  else
    assert(0 && "else");
}
// Imported from ESBMC regression/goto-coverage/github_2365_1
// Needs: --condition-coverage-claims --k-induction
// RUN: --condition-coverage-claims --k-induction
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  10
// EXPECT: ^Short Circuited Conditions:  4
// EXPECT: ^Total Conditions:  14
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
