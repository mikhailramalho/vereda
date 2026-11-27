/* github #9448: converting a NaN and an out-of-range float to int is
 * undefined (C11 6.3.1.4p1), so the two results need not agree. --fp2bv gave
 * every such conversion of one width the same value and proved a == b. */
#include <assert.h>
#include <math.h>

float __VERIFIER_nondet_float(void);

int main(void)
{
  float f1 = __VERIFIER_nondet_float(), f2 = __VERIFIER_nondet_float();
  __VERIFIER_assume(isnan(f1) && f2 > 1e30f);
  int a = (int)f1;
  int b = (int)f2;
  assert(a == b);
  return 0;
}
// Imported from ESBMC regression/floats/github_9448_fail
// RUN: --fp2bv
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
