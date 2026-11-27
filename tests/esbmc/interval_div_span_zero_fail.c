#include <assert.h>

/* A divisor range spanning 0 reaches the quotients of x == 1 and x == -1, not
 * only those of its endpoints. */
int main(void)
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= -2 && x <= 2 && x != 0);
  int y = 100 / x;
  if (y > 50)
    assert(0);
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_div_span_zero_fail
// Needs: --interval-analysis --interval-analysis-arithmetic
// RUN: --interval-analysis --interval-analysis-arithmetic --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
