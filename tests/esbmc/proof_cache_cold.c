#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  assert(x + 1 > x);
  return 0;
}
// Imported from ESBMC regression/esbmc/proof_cache_cold
// Needs: --proof-cache proof-store --claim-fingerprint-dump fp.tsv CHECK_FILE fp.tsv contains UNSAT
// RUN: --multi-property --proof-cache proof-store --claim-fingerprint-dump fp.tsv
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
