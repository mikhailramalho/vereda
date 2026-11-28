// atan keeps the sign of a zero, gives pi/2 with the sign of an infinity
// and propagates a NaN (C17 F.10.1.3), here for nondet arguments narrowed
// by assumptions; other results are within a few ulp of glibc's, here for
// volatile ones, which keep clang from folding the calls. atanf and atanl
// agree.
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern double __VERIFIER_nondet_double(void);
extern long double __VERIFIER_nondet_long_double(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  double x = __VERIFIER_nondet_double();
  __VERIFIER_assume(x == 0);
  double r = atan(x);
  __VERIFIER_assert(r == 0 && !signbit(r) == !signbit(x));

  x = __VERIFIER_nondet_double();
  __VERIFIER_assume(isinf(x));
  __VERIFIER_assert(atan(x) == (x > 0 ? M_PI_2 : -M_PI_2));

  x = __VERIFIER_nondet_double();
  __VERIFIER_assume(isnan(x));
  __VERIFIER_assert(isnan(atan(x)));

  long double l = __VERIFIER_nondet_long_double();
  __VERIFIER_assume(isinf(l) && l < 0);
  long double rl = atanl(l);
  __VERIFIER_assert(rl < -1.5707963267 && rl > -1.5707963268);

  volatile double one = 1, half = 0.5, two = 2, three = -3;
  volatile float f = 1;
  __VERIFIER_assert(atan(one) == M_PI_4 && atan(-one) == -M_PI_4);
  r = atan(half);
  __VERIFIER_assert(r > 0.4636476090008 && r < 0.4636476090009);
  r = atan(two);
  __VERIFIER_assert(r > 1.1071487177940 && r < 1.1071487177941);
  r = atan(three);
  __VERIFIER_assert(r < -1.2490457723982 && r > -1.2490457723983);
  __VERIFIER_assert(atanf(f) == 0x1.921fb6p-1f);
  return 0;
}
