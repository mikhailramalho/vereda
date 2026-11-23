#include <assert.h>
int main()
{
  int i, s = 0;
  for (i = 0; i < 4; i++)
  {
    if (i > 1)
      s += i;
    assert(s >= 0);
  }
  assert(i == 4);
  return 0;
}
// Imported from ESBMC regression/goto-coverage/github_6387_15
// Needs: --assertion-coverage --k-induction --base-k-step 2
// RUN: --multi-property --assertion-coverage --k-induction --base-k-step 2 --max-k-step 4
// MODE: FUTURE
// EXPECT: ^Coverage goals: 1 ✓ 1 reached$
// EXPECT: ^COVERAGE ANALYSIS INCOMPLETE
// EXPECT: reason: the run ended at the maximum k step
// EXPECT: \A(?![\s\S]*VERIFICATION FAILED)[\s\S]*\Z
// Vereda: verdict none
