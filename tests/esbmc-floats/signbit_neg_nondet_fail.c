#include <assert.h>
#include <math.h>

double __VERIFIER_nondet_double(void);

int main(void)
{
  double d = __VERIFIER_nondet_double();
  __VERIFIER_assume(d > 0);
  assert(signbit(-d) == 0);
  return 0;
}
// Imported from ESBMC regression/floats/signbit_neg_nondet_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
