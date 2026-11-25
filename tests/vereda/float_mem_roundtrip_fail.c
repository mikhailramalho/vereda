// Fail variant of float_mem_roundtrip.c: a symbolic -0.0 stored to memory
// and loaded back must keep its negative sign bit, so asserting !signbit
// fails.
// EXPECT: ^VERIFICATION FAILED$
#include <math.h>

extern double __VERIFIER_nondet_double(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);

int main(void) {
  double x = __VERIFIER_nondet_double();
  __VERIFIER_assume(x == 0.0 && signbit(x));
  volatile double v = x;
  double y = v;
  __VERIFIER_assert(!signbit(y));
  return 0;
}
