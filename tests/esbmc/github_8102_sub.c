#include <assert.h>

/* #8102: 0U - 1U wraps to UINT_MAX. */
int main(void)
{
  unsigned int x = __VERIFIER_nondet_uint();
  __VERIFIER_assume(x == 0);
  if (x - 1U != 4294967295U)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8102_sub
// Needs: --interval-analysis --interval-analysis-arithmetic
// RUN: --interval-analysis --interval-analysis-arithmetic
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
