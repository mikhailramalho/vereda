#include <fenv.h>
#include <math.h>

extern void __VERIFIER_error() __attribute__ ((__noreturn__));
void __VERIFIER_assert(int cond) { if (!(cond)) { ERROR: __VERIFIER_error(); } return; }

int main(void) 
{
  fesetround(FE_TONEAREST);
  __VERIFIER_assert(nearbyint(2.3) == 2.0);
  __VERIFIER_assert(nearbyint(2.5) == 2.0);
  __VERIFIER_assert(nearbyint(3.5) == 4.0);
  __VERIFIER_assert(nearbyint(-2.3) == -2.0);
  __VERIFIER_assert(nearbyint(-2.5) == -2.0);
  __VERIFIER_assert(nearbyint(-3.5) == -4.0);

  fesetround(FE_DOWNWARD);
  __VERIFIER_assert(nearbyint(2.3) == 2.0);
  __VERIFIER_assert(nearbyint(2.5) == 2.0);
  __VERIFIER_assert(nearbyint(3.5) == 3.0);
  __VERIFIER_assert(nearbyint(-2.3) == -3.0);
  __VERIFIER_assert(nearbyint(-2.5) == -3.0);
  __VERIFIER_assert(nearbyint(-3.5) == -4.0);

  __VERIFIER_assert(signbit(nearbyint(-0.0)));
  __VERIFIER_assert(nearbyint(-INFINITY) == -INFINITY);
}

// Imported from ESBMC regression/floats-regression/nearbyint2
// Needs: --interval-analysis
// RUN: --floatbv --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
