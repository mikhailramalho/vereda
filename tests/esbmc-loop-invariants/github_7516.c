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
  assert(s == 3);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7516
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --k-induction-parallel
// RUN: --loop-invariant-check --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^VERIFICATION UNKNOWN$
// EXPECT: \A(?![\s\S]*VERIFICATION FAILED)[\s\S]*\Z
