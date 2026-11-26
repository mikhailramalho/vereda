#include "assert.h"
int main()
{
  int x = 1;
  int y = 0;

  while(y < 1000 && __VERIFIER_nondet_int())
  {
    x = x + y;
    y = y + 1;
  }
  __VERIFIER_assert(y <= x);

  return 0;
}
// Imported from ESBMC regression/goto-contractor/github_731_01_VERIFIER_assert_success
// Needs: --interval-analysis --goto-contractor --incremental --unlimited-k-steps
// RUN: --interval-analysis --no-pointer-check --goto-contractor --incremental --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
