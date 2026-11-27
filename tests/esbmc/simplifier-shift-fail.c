#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x << 0 == x);
  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-shift-fail
// RUN: --ub-signed-overflow-check --ub-shift-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
