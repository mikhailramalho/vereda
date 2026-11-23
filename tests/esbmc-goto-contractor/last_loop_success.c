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
  while(x < 2147483640)
  {
    x = x + 1;
  }
  assert(x>0);

  return 0;
}
// Imported from ESBMC regression/goto-contractor/last_loop_success
// Needs: --interval-analysis --goto-contractor --incremental --unlimited-k-step
// RUN: --interval-analysis --no-pointer-check --goto-contractor --incremental --unlimited-k-step
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
