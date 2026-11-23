#include <assert.h>
int g[4];
int main(void)
{
  int *p = &g[0];
  unsigned char k = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(k < 2);
  ((unsigned char *)&p)[k] = 0x5a;
  assert(p == &g[0]);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7855-byte-update-sds_fail
// Needs: --smt-during-symex
// RUN: --64 --smt-during-symex
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
