#include <assert.h>

int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  int z = __VERIFIER_nondet_int();
  __VERIFIER_assume(x < 5);
  assert(x != 5);
  if (y > 0)
  {
    assert(x != 5);
    if (z > 0)
      assert(x != 5);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_1211
// RUN: --no-incremental-bmc
// EXPECT: ^Caching time: .*\(removed 2 assertions\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^Caching time: .*\(removed 2 assertions\)$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
