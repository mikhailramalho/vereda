#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  assert(x + 1 > x);
  return 0;
}
// Imported from ESBMC regression/esbmc/proof_cache_verify_without_cache
// Needs: --proof-cache-verify
// RUN: --multi-property --proof-cache-verify --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ERROR: --proof-cache-verify has no cache to check without --proof-cache <dir>$
// Vereda: verdict none
