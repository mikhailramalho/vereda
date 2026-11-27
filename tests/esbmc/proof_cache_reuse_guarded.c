#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  if (x < 100)
    assert(x + 1 > x);
  if (x < 100)
    assert(x + 1 > x);
  return 0;
}
// Imported from ESBMC regression/esbmc/proof_cache_reuse_guarded
// Needs: --proof-cache proof-store fp.tsv
// RUN: --multi-property --proof-cache proof-store fp.tsv
// MODE: FUTURE
// EXPECT: ^Proof cache: 1 claim\(s\) reused, 1 solved$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
