/* Neither the forward condition nor the inductive step can settle this loop,
   so the k steps run out and the driver reports the table itself. */
#include <assert.h>

int main()
{
  int x = 0;
  while (1)
  {
    assert(x < 3);
    assert(x != 100);
    ++x;
  }
}
// Imported from ESBMC regression/esbmc/multi_property_kinduction_exhausted_k
// Needs: --k-induction
// RUN: --k-induction --multi-property --max-k-step 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Properties: 2 checked, \d+ passed, 1 failed, \d+ not checked, 1 unknown$
// EXPECT: \A(?![\s\S]*VERIFICATION SUCCESSFUL)[\s\S]*\Z
// Vereda: verdict none
