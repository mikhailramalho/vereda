#include <assert.h>

int main()
{
  unsigned int x = 100;
  while(x>0)
  {
   __VERIFIER_assume(x>100);
   x--;
  }
  assert(0);
  return 0;
}
// Imported from ESBMC regression/incremental-smt/incremental-12
// Needs: --smt-symex-assert
// RUN: --smt-symex-assert --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
