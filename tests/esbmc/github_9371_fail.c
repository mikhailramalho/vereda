/* #9371: an infinity times a nonzero finite number is an infinity
   (C17 G.5.1p4), so asserting a product with no infinite part fails. */
#include <assert.h>
#include <math.h>

double __VERIFIER_nondet_double(void);

int main(void)
{
  double _Complex a, b;
  __real__ a = INFINITY;
  __imag__ a = INFINITY;
  __real__ b = __VERIFIER_nondet_double();
  __imag__ b = 0.0;
  __VERIFIER_assume(isfinite(__real__ b) && __real__ b != 0);
  double _Complex r = a * b;
  assert(!(isinf(__real__ r) || isinf(__imag__ r)));
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9371_fail
// RUN: -m64
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
