/* remquo's remainder obeys the same |r| <= |y|/2 bound as remainder(). */
#include <assert.h>

double __VERIFIER_nondet_double(void);
#include <math.h>
int main(void)
{
  double x = __VERIFIER_nondet_double();
  double y = __VERIFIER_nondet_double();
  __VERIFIER_assume(isgreaterequal(x, -1e6) && islessequal(x, 1e6));
  __VERIFIER_assume(isgreaterequal(y, 1.0) && islessequal(y, 1024.0));
  int q;
  double r = remquo(x, y, &q);
  assert(islessequal(fabs(r), fabs(y) * 0.5));
  return 0;
}
// Imported from ESBMC regression/floats-regression/ieee_rem_remquo_bound
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
