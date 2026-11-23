#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x * (-1) == -x);
  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-mult-fail
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
