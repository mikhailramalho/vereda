/* #7516: a counterexample against the loop-invariant abstraction leaves the
   base case unknown under a k-step strategy. */
#include <assert.h>
int main()
{
  int i = 0, s = 0;
  __ESBMC_loop_invariant(i >= 0);
  while (i < 3)
  {
    s++;
    i++;
  }
  assert(s == 4);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7516_kinduction
// Needs: --loop-invariant-check --k-induction
// RUN: --loop-invariant-check --k-induction --max-k-step 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// EXPECT: \A(?![\s\S]*VERIFICATION SUCCESSFUL)[\s\S]*\Z
// EXPECT: \A(?![\s\S]*VERIFICATION UNKNOWN[\s\S]*VERIFICATION UNKNOWN)[\s\S]*\Z
// Vereda: verdict none
