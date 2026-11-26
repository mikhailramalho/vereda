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
// RUN: --incremental-bmc --unlimited-k-steps --solver z3
// EXPECT: ^VERIFICATION FAILED$
