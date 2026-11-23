#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x << 0 == x);
  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-shift-fail
// RUN: --ub-shift-check
// EXPECT: ^VERIFICATION FAILED$
