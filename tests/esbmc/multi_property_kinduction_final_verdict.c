/* esbmc/esbmc#7900 D1: the loop cannot be exhausted, so no phase settles the
   program and the run reaches --max-k-step with a violation already reported.
   The verdict must follow the counterexample, not the exhausted bound. */
#include <assert.h>
int main()
{
  int x = 0;
  while (1)
  {
    assert(x < 3);
    assert(x != 10);
    ++x;
  }
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_final_verdict
// Needs: --k-induction
// RUN: --k-induction --multi-property --max-k-step 6
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*Unable to prove or falsify the program)[\s\S]*\Z
// Vereda: verdict none
