// fmodf(x, 3) is in [2, 3) for some x (2, 5, 2.5, ...), so this fails;
// SMT's fp.rem alone, which rounds the quotient to nearest, would never
// exceed 1.5 here.
// EXPECT: ^VERIFICATION FAILED$
#include <math.h>
extern float __VERIFIER_nondet_float(void);
extern void __VERIFIER_assert(int);
int main(void) {
  float x = __VERIFIER_nondet_float();
  float r = fmodf(x, 3.0f);
  __VERIFIER_assert(!(r >= 2.0f));
  return 0;
}
