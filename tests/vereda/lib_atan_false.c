// atan(-0) is -0 (C17 F.10.1.3), so its sign bit is set for that zero only.
// EXPECT: ^VERIFICATION FAILED$
#include <math.h>
extern double __VERIFIER_nondet_double(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  double x = __VERIFIER_nondet_double();
  __VERIFIER_assume(x == 0 || x == 1);
  __VERIFIER_assert(!signbit(atan(x)));
  return 0;
}
