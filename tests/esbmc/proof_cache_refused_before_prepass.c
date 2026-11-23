#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  assert(x + 1 > x);
  return 0;
}
// Imported from ESBMC regression/esbmc/proof_cache_refused_before_prepass
// Needs: --proof-cache proof-store --falsify-context-bound 2
// RUN: --proof-cache proof-store --falsify-context-bound 2
// MODE: FUTURE
// EXPECT: ^ERROR: --proof-cache reuses claims one at a time and needs --multi-property$
// Vereda: verdict none
