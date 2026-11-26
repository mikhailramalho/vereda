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
    x++;
  }
  assert(x != y);
  return 0;
}
// Imported from ESBMC regression/goto-contractor/operator_neq_success
// Needs: --interval-analysis --goto-contractor
// RUN: --interval-analysis --no-pointer-check --goto-contractor --incremental-bmc --unlimited-k-steps --k-step 100
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
