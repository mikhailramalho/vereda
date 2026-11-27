#include <assert.h>

/* #8102: UINT_MAX + 1U wraps to 0. */
int main(void)
{
  unsigned int x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x == 4294967295U);
  if (x + 1U != 0)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8102
// Needs: --interval-analysis --interval-analysis-arithmetic
// RUN: --interval-analysis --interval-analysis-arithmetic --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
