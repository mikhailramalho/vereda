#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  assert(x + 1 > x);
  return 0;
}
// Imported from ESBMC regression/esbmc/proof_cache_without_multi_property
// Needs: --proof-cache proof-store
// RUN: --proof-cache proof-store
// MODE: FUTURE
// EXPECT: ^ERROR: --proof-cache reuses claims one at a time and needs --multi-property$
// Vereda: verdict none
