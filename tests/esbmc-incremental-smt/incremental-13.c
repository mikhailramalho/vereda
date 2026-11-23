#include <assert.h>

int main()
{
  unsigned int x = 100;
  while(x>0)
  {
   __VERIFIER_assume(x>0);
   x--;
  }
  assert(0);
  return 0;
}
// Imported from ESBMC regression/incremental-smt/incremental-13
// Needs: --smt-during-symex --smt-symex-assert
// RUN: --smt-during-symex --smt-symex-assert --incremental-bmc --unlimited-k-steps --z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
