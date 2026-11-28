// As fenv_rounding_nondet, but expecting toward zero to give the float
// quotient nearest gives: wrong only for that mode (#301).
// EXPECT: ^  FAILED +\[main\.assertion\.5\]  line 27  __VERIFIER_assert$
// EXPECT: ^VERIFICATION FAILED$
#include <fenv.h>
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);

int main(void) {
  int m = __VERIFIER_nondet_int();
  int mode = m == 0 ? FE_TONEAREST : m == 1 ? FE_DOWNWARD : m == 2 ? FE_UPWARD : FE_TOWARDZERO;
  volatile double two = 2.0, three = 3.0, minus = -1.0;
  volatile float ftwo = 2.0f, fthree = 3.0f;
  __VERIFIER_assert(fesetround(mode) == 0);
  double a = two / three, b = minus / three;
  float f = ftwo / fthree;
  if (mode == FE_TONEAREST)
    __VERIFIER_assert(a == 0x1.5555555555555p-1 && b == -0x1.5555555555555p-2 &&
                      f == 0x1.555556p-1f);
  if (mode == FE_DOWNWARD)
    __VERIFIER_assert(a == 0x1.5555555555555p-1 && b == -0x1.5555555555556p-2 &&
                      f == 0x1.555554p-1f);
  if (mode == FE_UPWARD)
    __VERIFIER_assert(a == 0x1.5555555555556p-1 && b == -0x1.5555555555555p-2 &&
                      f == 0x1.555556p-1f);
  if (mode == FE_TOWARDZERO)
    __VERIFIER_assert(a == 0x1.5555555555555p-1 && b == -0x1.5555555555555p-2 &&
                      f == 0x1.555556p-1f);
  return 0;
}
