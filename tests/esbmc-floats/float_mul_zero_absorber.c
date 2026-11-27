#include <assert.h>
#include <math.h>

float __VERIFIER_nondet_float(void);

/* x * 0 is not 0 under IEEE 754: it is NaN when x is infinite or NaN
 * (IEEE 754-2019 sec 7.2), and the sign of a zero product is the XOR of the
 * operand signs (sec 6.3). Both operand orders, because a fold that inspects
 * only one side is caught by only one of them. */
int main()
{
  float i = __VERIFIER_nondet_float();
  __VERIFIER_assume(isinf(i));
  assert(isnan(i * 0.0f));
  assert(isnan(0.0f * i));

  float n = __VERIFIER_nondet_float();
  __VERIFIER_assume(isnan(n));
  assert(isnan(n * 0.0f));
  assert(isnan(0.0f * n));

  float a = __VERIFIER_nondet_float();
  __VERIFIER_assume(a < 0.0f && isfinite(a));
  assert(signbit(a * 0.0f));
  assert(signbit(0.0f * a));

  return 0;
}
// Imported from ESBMC regression/floats/float_mul_zero_absorber
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*FAILED)[\s\S]*\Z
