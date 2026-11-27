#include <assert.h>
int main()
{
  int x = 1;
  int y = 0;

  while(y < 1000 && __VERIFIER_nondet_int())
  {
    x = x + y;
    y = y + 1;
  }
  assert(x*2 >= y);

  return 0;
}
// Imported from ESBMC regression/goto-contractor/github_650_05_operator_mul_success
// Needs: --interval-analysis --goto-contractor --incremental
// RUN: --interval-analysis --ub-pointer-check=false --goto-contractor --incremental --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
