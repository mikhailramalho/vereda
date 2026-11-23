#include <assert.h>

int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > 0 && x < 10);
  assert(x > 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/claim_fingerprint_dump_fail
// Needs: --claim-fingerprint-dump -
// RUN: --multi-property --claim-fingerprint-dump -
// MODE: FUTURE
// EXPECT: ^CLAIM-FP [0-9a-f]{16}\s+[0-9a-f]{16}\s+[0-9a-f]{16}\s+[0-9]+\s+SAT\s
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
