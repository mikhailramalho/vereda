#include <assert.h>

int main()
{
  int x;
  unsigned int y;
  __VERIFIER_assume(x >= -2147483648 && x <= 2147483647);
  __VERIFIER_assume(y >= 0 && y <= 4294967295);

  if (x < 1 && y < 1)
  {
    x = x + y;
  }
  else if (x >= 0)
    x++;

  __VERIFIER_assume(y >= 2147483648 && y <= 4294967295);
  if (x > 0)
    assert(x < y);

  return 0;
}
// Imported from ESBMC regression/Interval-analysis-ibex-contractor/14-PR1638-success
// Needs: --interval-analysis --interval-analysis-ibex-contractor --k-induction
// RUN: --interval-analysis --no-pointer-check --interval-analysis-ibex-contractor --k-induction --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
