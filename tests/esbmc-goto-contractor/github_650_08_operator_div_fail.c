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
  assert(x/2 >= y);

  return 0;
}
// Imported from ESBMC regression/goto-contractor/github_650_08_operator_div_fail
// Needs: --interval-analysis --goto-contractor --incremental
// RUN: --interval-analysis --no-pointer-check --goto-contractor --incremental --unlimited-k-steps
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
