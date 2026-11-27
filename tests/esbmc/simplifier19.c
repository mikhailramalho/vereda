#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();

  // (~x) == (~y) -> x == y
  assert(((~x) == (~y)) == (x == y));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier19
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
