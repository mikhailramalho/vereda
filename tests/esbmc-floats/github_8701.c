/* Issue #8701: fesetround of an unrecognised mode must fail and leave the
 * rounding direction unchanged (C11 7.6.3.2). */
#include <assert.h>
#include <fenv.h>
int main(void)
{
  assert(fesetround(FE_UPWARD) == 0);
  assert(fesetround(12345) != 0);
  assert(fegetround() == FE_UPWARD);
  assert(fesetround(FE_TONEAREST) == 0);
  assert(fegetround() == FE_TONEAREST);
  return 0;
}
// Imported from ESBMC regression/floats/github_8701
// RUN: --multi-property --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
