/* The bit-vector encoding builds a concrete NaN bit-pattern, so it keeps the
 * sign the native floating-point theory cannot represent (#7021). */
#include <assert.h>
#include <math.h>

extern double __VERIFIER_nondet_double(void);

int main(void)
{
  double m = __VERIFIER_nondet_double();
  __VERIFIER_assume(m < 0.0);
  double s = copysign(NAN, m);
  assert(isnan(s) && signbit(s));
  return 0;
}
// Imported from ESBMC regression/bitwuzla/fp2bv_nan_sign
// RUN: --solver bitwuzla --fp2bv
// EXPECT: ^VERIFICATION SUCCESSFUL$
