#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  assert(x + 1 > x);
  return 0;
}
// Imported from ESBMC regression/esbmc/claim_fingerprint_dump
// Needs: --claim-fingerprint-dump -
// RUN: --multi-property --claim-fingerprint-dump -
// MODE: FUTURE
// EXPECT: ^CLAIM-FP [0-9a-f]{16}\s+[0-9a-f]{16}\s+[0-9a-f]{16}\s+[0-9]+\s+UNSAT\s
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
