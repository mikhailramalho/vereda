#include <assert.h>
int main()
{
  int x, y;
  x = 1;
  y = 0;
  __VERIFIER_assume(x >= 0);
  __VERIFIER_assume(y >= 0);
  while(y <= 1000 && __VERIFIER_nondet_int())
  {
    y++;
    x += y;
  }
  assert(-x >= -y);
  return 0;
}
// Imported from ESBMC regression/goto-contractor/operator_neg_fail
// Needs: --interval-analysis --goto-contractor --unlimited-k-steps
// RUN: --interval-analysis --no-pointer-check --goto-contractor --incremental-bmc --unlimited-k-steps --k-step 100
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
