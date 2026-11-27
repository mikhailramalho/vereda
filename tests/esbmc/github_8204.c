#include <assert.h>
int __VERIFIER_nondet_int(void);

/* #8204: x is [0, 2^31-1] or [2^64-2^31, 2^64-1]; the first assume keeps
 * [0, 2^31-1], and x < 10 leaves v in [0, 9]. */
int main()
{
  int v = __VERIFIER_nondet_int();
  unsigned long x = v;
  __VERIFIER_assume(x < (unsigned long)(-2147483647));
  __VERIFIER_assume(x < 10);
  assert(x < 10 && v >= 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8204
// Needs: --interval-analysis --interval-analysis-wrapped
// RUN: --interval-analysis --interval-analysis-wrapped --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
