#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  assert(x > 5);
  assert(x > 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/proof_cache_reuse_fail
// Needs: --proof-cache proof-store fp.tsv
// RUN: --multi-property --proof-cache proof-store fp.tsv --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Proof cache: 0 claim\(s\) reused, 2 solved$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
