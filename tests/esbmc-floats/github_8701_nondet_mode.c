/* Issue #8701: whatever value fesetround rejects, the rounding direction is
 * left as it was (C11 7.6.3.2p2). */
#include <assert.h>
#include <fenv.h>
int __VERIFIER_nondet_int();
int main(void)
{
  assert(fesetround(FE_UPWARD) == 0);
  int mode = __VERIFIER_nondet_int();
  if (fesetround(mode) != 0)
    assert(fegetround() == FE_UPWARD);
  return 0;
}
// Imported from ESBMC regression/floats/github_8701_nondet_mode
// RUN: --multi-property
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
