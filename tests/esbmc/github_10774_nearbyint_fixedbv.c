// nearbyint and the rint family it backs follow the current rounding mode.
#include <fenv.h>
#include <math.h>
int main()
{
  __VERIFIER_assert(nearbyint(2.5) == 2.0); // nearest: tie to even
  __VERIFIER_assert(nearbyint(3.5) == 4.0); // nearest: tie to even, up
  __VERIFIER_assert(nearbyint(-2.5) == -2.0); // nearest: negative tie
  __VERIFIER_assert(rint(2.4) == 2.0 && llrint(5.0 / 3.0) == 2); // rint, llrint
  fesetround(FE_UPWARD);
  __VERIFIER_assert(nearbyint(2.1) == 3.0 && nearbyint(-2.9) == -2.0); // upward
  fesetround(FE_DOWNWARD);
  __VERIFIER_assert(nearbyint(2.9) == 2.0 && nearbyint(-2.1) == -3.0); // downward
  fesetround(FE_TOWARDZERO);
  __VERIFIER_assert(nearbyint(-2.7) == -2.0 && lrint(2.7) == 2); // toward zero
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10774_nearbyint_fixedbv
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict FAILED
