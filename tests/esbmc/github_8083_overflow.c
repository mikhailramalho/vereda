#include <assert.h>

/* The overflow builtins write their result through a pointer. */
int main()
{
  int r = 5;
  int a = __VERIFIER_nondet_int();
  __builtin_sadd_overflow(a, 1, &r);
  assert(r == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8083_overflow
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
