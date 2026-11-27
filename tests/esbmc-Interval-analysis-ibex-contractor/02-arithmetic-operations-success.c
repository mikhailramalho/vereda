#include <assert.h>

int main()
{
  int x = 100;
  float y = 0.1;

  if(y / 5 + 2 * x - 7 < 1)
  {
    x = x + y;
    y = y + 1;
  }
  while(x / 2 - 10 * y < 20)
    x++;

  assert(10 * x > 0);

  return 0;
}
// Imported from ESBMC regression/Interval-analysis-ibex-contractor/02-arithmetic-operations-success
// Needs: --interval-analysis --interval-analysis-ibex-contractor --k-induction
// RUN: --interval-analysis --ub-pointer-check=false --interval-analysis-ibex-contractor --k-induction --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
