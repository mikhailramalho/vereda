#include <assert.h>

int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();

  // Same reassoc shape as the passing test, but with a wrong expected value.
  // (x + y) + (-x) reassociates to y; assert against y+1 must fail.
  assert(((x + y) + (-x)) == (y + 1));

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-add-reassoc-fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
