#include <assert.h>

int main()
{
  int x=__VERIFIER_nondet_int();
  while(x>0)
  {
   __VERIFIER_assume(x>100);
   x--;
  }
  assert(0);
  return 0;
}
// Imported from ESBMC regression/incremental-smt/incremental-11
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
