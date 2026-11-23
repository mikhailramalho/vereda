#include <assert.h>
#include <stdbool.h>

int b;
int main()
{
  int a = 1;
  // if (a == 1 && b == 2) { // |1|
  if ((a == 1 || b == 2) != 0)
  { // |2|
    assert(0 && "if");
  }
  else
    assert(0 && "else");
}
// Imported from ESBMC regression/goto-coverage/github_2365_2
// Needs: --condition-coverage-claims --k-induction
// RUN: --condition-coverage-claims --k-induction
// MODE: FUTURE
// EXPECT: ^Reached Conditions:  6
// EXPECT: ^Short Circuited Conditions:  8
// EXPECT: ^Total Conditions:  14
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
