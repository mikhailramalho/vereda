/* fesetround writes a model variable the havoc does not cover, so the
   inductive step must still follow the paths the bound cuts out of the loop
   (esbmc/esbmc#1320). */
#include <fenv.h>
int main()
{
  unsigned i = 0;
  while (i < 20)
  {
    if (i == 0)
      fesetround(FE_UPWARD);
    i++;
  }
  __VERIFIER_assert(fegetround() == FE_TONEAREST); // rounding mode restored
  return 0;
}
// Imported from ESBMC regression/k-induction/github_1320_fesetround_fail
// Needs: --k-induction
// RUN: --k-induction --max-k-step 25
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
