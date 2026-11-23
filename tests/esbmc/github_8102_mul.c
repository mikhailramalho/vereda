#include <assert.h>

/* #8102: 65536 * 65536 wraps to 0 in a 32-bit int (--overflow-check is off). */
int main(void)
{
  int s = __VERIFIER_nondet_int();
  __VERIFIER_assume(s == 65536);
  if (s * s != 0)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8102_mul
// Needs: --interval-analysis --interval-analysis-arithmetic
// RUN: --interval-analysis --interval-analysis-arithmetic
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
