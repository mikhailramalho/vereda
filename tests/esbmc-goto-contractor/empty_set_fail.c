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
  assert(x<0);

  return 0;
}
// Imported from ESBMC regression/goto-contractor/empty_set_fail
// Needs: --interval-analysis --goto-contractor --incremental --unlimited-k-steps
// RUN: --interval-analysis --no-pointer-check --goto-contractor --incremental --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
