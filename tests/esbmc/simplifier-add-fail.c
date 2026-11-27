#include <assert.h>

int main()
{
  int x = __VERIFIER_nondet_int();
  assert(x + (-x) == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-add-fail
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
