// #7971
#include <assert.h>

int main()
{
  int x = 0;
  while (1)
  {
    assert(x >= 0);
    assert(x != 10);
    x = 1;
  }
}
// Imported from ESBMC regression/k-induction/github_7971_safe
// Needs: --k-induction --interval-analysis
// RUN: --k-induction --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
