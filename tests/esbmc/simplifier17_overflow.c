#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();

  // (-x) == (-y) -> x == y
  assert(((-x) == (-y)) == (x == y));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier17_overflow
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
