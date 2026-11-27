#include <assert.h>

int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  int w = __VERIFIER_nondet_int();

  // (x + y) + w - x - y reassociates to w; w == w + 1 must fail.
  int a = (x + y) + w - x - y;
  assert(a == w + 1);

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-reassoc-cancel-fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
