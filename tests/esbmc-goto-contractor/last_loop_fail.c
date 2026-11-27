#include <assert.h>
int main()
{
  int x = 1;
  int y = 0;

  while(y<10)
  {
    y++;
  }

  __VERIFIER_assume(x>0);
  while(x < 2147483640 && __VERIFIER_nondet_int())
  {
    x = x + 1;
  }
  assert(x>10);

  return 0;
}
// Imported from ESBMC regression/goto-contractor/last_loop_fail
// Needs: --interval-analysis --goto-contractor --incremental
// RUN: --interval-analysis --ub-pointer-check=false --goto-contractor --incremental --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
