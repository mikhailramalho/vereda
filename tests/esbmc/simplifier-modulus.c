#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x!=0);
  assert(x % x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-modulus
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
