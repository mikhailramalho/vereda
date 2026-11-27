#include <assert.h>
#include <math.h>

float __VERIFIER_nondet_float(void);

int main()
{
  float i = __VERIFIER_nondet_float();
  __VERIFIER_assume(isinf(i));
  assert(i * 0.0f == 0.0f);
  return 0;
}
// Imported from ESBMC regression/floats/float_mul_zero_absorber_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
