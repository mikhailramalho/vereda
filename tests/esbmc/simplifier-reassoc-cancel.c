#include <assert.h>

int __VERIFIER_nondet_int(void);

int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  int w = __VERIFIER_nondet_int();

  // X + (-X) cancellation across a chain.
  // (x + y) + w - x - y should reassociate down to w.
  int a = (x + y) + w - x - y;
  assert(a == w);

  // Constant cancels constant; variables cancel pairwise.
  // 5 + x - 5 + y - x reassociates to y.
  int b = 5 + x - 5 + y - x;
  assert(b == y);

  // Triple cancellation.
  // x - x + y - y + 3 reassociates to 3.
  int c = x - x + y - y + 3;
  assert(c == 3);

  return 0;
}
// Imported from ESBMC regression/esbmc/simplifier-reassoc-cancel
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
