#include <assert.h>
#include <math.h>

double __VERIFIER_nondet_double(void);

int main(void)
{
  double d = __VERIFIER_nondet_double();
  __VERIFIER_assume(d > 0);
  assert(signbit(-d) == 1);
  assert(signbit(fabs(-d)) == 0);
  assert(signbit(-0.0) == 1);
  return 0;
}
// Imported from ESBMC regression/floats/signbit_neg_nondet
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
