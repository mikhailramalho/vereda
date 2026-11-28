// fesetround sets the mode fegetround reads, refuses a value that is no
// mode, and the divisions after it round up and then down: a native run
// built with -frounding-math gives these quotients (#301).
// EXPECT: ^VERIFICATION SUCCESSFUL$
#include <fenv.h>
extern void __VERIFIER_assert(int);

int main(void) {
  volatile double one = 1.0, three = 3.0;
  volatile float fone = 1.0f, fthree = 3.0f;
  __VERIFIER_assert(fegetround() == FE_TONEAREST);
  __VERIFIER_assert(fesetround(FE_UPWARD) == 0 && fegetround() == FE_UPWARD);
  double up = one / three;
  float fup = fone / fthree;
  __VERIFIER_assert(fesetround(1) != 0 && fegetround() == FE_UPWARD);
  __VERIFIER_assert(fesetround(FE_DOWNWARD) == 0 && fegetround() == FE_DOWNWARD);
  double down = one / three;
  float fdown = fone / fthree;
  __VERIFIER_assert(up == 0x1.5555555555556p-2 && down == 0x1.5555555555555p-2);
  __VERIFIER_assert(fup == 0x1.555556p-2f && fdown == 0x1.555554p-2f);
  return 0;
}
