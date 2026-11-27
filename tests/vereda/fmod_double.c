// fmod on doubles, as fmodf: the quotient truncates, a zero result has x's
// sign, and a quotient of about 1.4e299 still gives the exact remainder.
// The expected values are glibc's.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern double __VERIFIER_nondet_double(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  double x = __VERIFIER_nondet_double();
  __VERIFIER_assume(x == 5.0 || x == -5.0);
  double r = fmod(x, 3.0);
  __VERIFIER_assert(r == (x > 0 ? 2.0 : -2.0));

  x = __VERIFIER_nondet_double();
  __VERIFIER_assume(x == -6.0);
  r = fmod(x, 3.0);
  __VERIFIER_assert(r == 0.0 && signbit(r));

  x = __VERIFIER_nondet_double();
  __VERIFIER_assume(x == 1e300 || x == -1e300);
  r = fmod(x, 7.0);
  __VERIFIER_assert(r == (x > 0 ? 1.0 : -1.0));
  return 0;
}
