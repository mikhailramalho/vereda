// fmodf truncates the quotient, unlike SMT's fp.rem, which rounds it to
// nearest (remainderf(5, 3) is -1, fmodf(5, 3) is 2). The operands are
// nondet floats narrowed by assumptions, so the encoder computes each
// remainder; the expected values are glibc's.
// RUN:
// RUN: --fp2bv
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <math.h>
extern float __VERIFIER_nondet_float(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  float x = __VERIFIER_nondet_float(), y = __VERIFIER_nondet_float();
  __VERIFIER_assume((x == 5.0f || x == -5.0f) && (y == 3.0f || y == -3.0f));
  float r = fmodf(x, y);
  __VERIFIER_assert(r == (x > 0 ? 2.0f : -2.0f));

  x = __VERIFIER_nondet_float();
  __VERIFIER_assume(x == 7.5f || x == -7.5f);
  r = fmodf(x, 2.0f);
  __VERIFIER_assert(r == (x > 0 ? 1.5f : -1.5f));

  x = __VERIFIER_nondet_float(); // quotients 1.5 and 2.5: ties
  __VERIFIER_assume(x == 3.0f || x == 5.0f);
  r = fmodf(x, 2.0f);
  __VERIFIER_assert(r == 1.0f);

  x = __VERIFIER_nondet_float(); // a zero result has x's sign
  __VERIFIER_assume(x == 6.0f || x == -6.0f || x == 0.0f);
  r = fmodf(x, 3.0f);
  __VERIFIER_assert(r == 0.0f && !signbit(r) == !signbit(x));

  x = __VERIFIER_nondet_float();
  __VERIFIER_assume(isinf(x));
  __VERIFIER_assert(isnan(fmodf(x, 3.0f)));

  x = __VERIFIER_nondet_float(), y = __VERIFIER_nondet_float();
  __VERIFIER_assume(y == 0.0f);
  __VERIFIER_assert(isnan(fmodf(x, y)));

  x = __VERIFIER_nondet_float(), y = __VERIFIER_nondet_float();
  __VERIFIER_assume(isnan(x) || isnan(y));
  __VERIFIER_assert(isnan(fmodf(x, y)));

  x = __VERIFIER_nondet_float(), y = __VERIFIER_nondet_float();
  __VERIFIER_assume(isfinite(x) && isinf(y));
  r = fmodf(x, y);
  __VERIFIER_assert(r == x && !signbit(r) == !signbit(x));

  x = __VERIFIER_nondet_float(); // a quotient of about 1.4e29
  __VERIFIER_assume(x == 1e30f || x == -1e30f);
  r = fmodf(x, 7.0f);
  __VERIFIER_assert(r == (x > 0 ? 1.0f : -1.0f));

  x = __VERIFIER_nondet_float(); // subnormal
  __VERIFIER_assume(x == 1e-40f || x == -1e-40f);
  __VERIFIER_assert(fmodf(x, 3.0f) == x);
  return 0;
}
