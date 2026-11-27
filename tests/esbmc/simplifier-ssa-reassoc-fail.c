#include <assert.h>

int __VERIFIER_nondet_int(void);

static int compute(int a, int b)
{
  return 5 - a - 8 - b - 2;
}

int main()
{
  int x = __VERIFIER_nondet_int();

  // Same shape as the passing test, but with an off-by-one expectation —
  // must fail.
  int r = compute(10, x);
  assert(r == -14 - x);

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-ssa-reassoc-fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
