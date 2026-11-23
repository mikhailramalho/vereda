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
  assert(x+5 >= y);

  return 0;
}
// Imported from ESBMC regression/goto-contractor/github_650_01_operator_add_success
// Needs: --interval-analysis --goto-contractor --incremental --unlimited-k-step
// RUN: --interval-analysis --no-pointer-check --goto-contractor --incremental --unlimited-k-step
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
