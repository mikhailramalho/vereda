#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  assert(x + 1 > x);
  return 0;
}
// Imported from ESBMC regression/esbmc/proof_cache_inactive_coverage
// Needs: --assertion-coverage --proof-cache proof-store
// RUN: --assertion-coverage --proof-cache proof-store
// MODE: FUTURE
// EXPECT: ^WARNING: Proof cache: inactive \(coverage measurement\); every claim will be solved and none stored$
// Vereda: verdict none
