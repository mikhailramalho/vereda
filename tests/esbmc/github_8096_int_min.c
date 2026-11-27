#include <assert.h>

/* #8096: negating INT_MIN wraps back to INT_MIN (--overflow-check is off). */
int main(void)
{
  int s = __VERIFIER_nondet_int();
  __VERIFIER_assume(s == -2147483647 - 1);
  int r = -s;
  if (r != s)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8096_int_min
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
