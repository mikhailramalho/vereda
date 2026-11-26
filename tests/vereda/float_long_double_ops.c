// x86_fp80 (long double) arithmetic, casts, intrinsics, classes, subnormals,
// and memory round-trip across -m64, -m64 --fp2bv, and -m32.
// RUN: -m64
// RUN: -m64 --fp2bv
// RUN: -m32
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <float.h>
#include <math.h>
#include <stdint.h>

extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern long double __VERIFIER_nondet_long_double(void);
extern double __VERIFIER_nondet_double(void);
extern int __VERIFIER_nondet_int(void);

int main(void) {
  // Symbolic arithmetic and precision beyond double (64-bit vs 53-bit significand).
  long double x = __VERIFIER_nondet_long_double();
  __VERIFIER_assume(x == 1.0L / 3.0L);
  __VERIFIER_assert((double)x != x);
  __VERIFIER_assert(x + x == 2.0L / 3.0L);
  __VERIFIER_assert(x * 3.0L == 1.0L);
  __VERIFIER_assert((-x) < 0.0L);
  __VERIFIER_assert(fabsl(-x) == x);

  // Mixed native-FP (double) <-> BV-FP (x86_fp80) conversion on symbolic values.
  double d = __VERIFIER_nondet_double();
  __VERIFIER_assume(d == 1.5);
  long double ld = (long double)d;
  __VERIFIER_assert(ld == 1.5L);
  __VERIFIER_assert((double)(ld + 0.5L) == 2.0);

  // Integer <-> x86_fp80 conversions.
  int k = __VERIFIER_nondet_int();
  __VERIFIER_assume(k == -42);
  long double lk = (long double)k;
  __VERIFIER_assert(lk == -42.0L);
  __VERIFIER_assert((int)lk == -42);

  // Special values and classes: zero, subnormal, infinity, NaN.
  long double zero = 0.0L;
  long double nzero = -0.0L;
  long double sub = LDBL_TRUE_MIN;
  long double inf = (long double)INFINITY;
  long double nanv = (long double)NAN;
  __VERIFIER_assert(zero == nzero);
  __VERIFIER_assert(signbit(nzero) && !signbit(zero));
  __VERIFIER_assert(fpclassify(sub) == FP_SUBNORMAL);
  __VERIFIER_assert(isinf(inf) && !isfinite(inf));
  __VERIFIER_assert(isnan(nanv) && nanv != nanv);

  // Libm intrinsics on x86_fp80.
  long double s = __VERIFIER_nondet_long_double();
  __VERIFIER_assume(s == 9.0L);
  __VERIFIER_assert(sqrtl(s) == 3.0L);
  __VERIFIER_assert(floorl(2.75L) == 2.0L);
  __VERIFIER_assert(ceill(2.25L) == 3.0L);

  // Memory store/load round-trip (10-byte store size, explicit integer bit).
  volatile long double slot = sub;
  long double back = slot;
  __VERIFIER_assert(back == LDBL_TRUE_MIN);
  slot = x;
  back = slot;
  __VERIFIER_assert(back == x);

  return 0;
}
