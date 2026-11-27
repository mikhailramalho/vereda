#include <assert.h>

/* #8096: -1U is 4294967295U, so the branch is taken. */
int main(void)
{
  unsigned int x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x == 4294967295U);
  if (x == -1U)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8096_fail
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
