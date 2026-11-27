#include <assert.h>

int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  __VERIFIER_assume(y <= 0 || x != 5);
  if (y > 0)
    assert(x != 5);
  assert(x != 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1211_order_fail
// RUN: --no-incremental-bmc
// EXPECT: ^Caching time: .*\(removed 0 assertions\)$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^Caching time: .*\(removed 0 assertions\)$
// MODE: FUTURE
// Vereda: verdict FAILED
