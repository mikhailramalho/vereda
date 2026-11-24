/* #7516: a counterexample against the loop-invariant abstraction leaves the
   base case unknown under a k-step strategy. */
#include <assert.h>
int main()
{
  int i = 0, s = 0;
  __ESBMC_loop_invariant(s == i && i <= 3);
  while (i < 3)
  {
    s++;
    i++;
  }
  assert(s == 4);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7516_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --k-induction-parallel
// RUN: --loop-invariant-check --k-induction-parallel
// MODE: FUTURE
// EXPECT: ^Bug found by the base case \(k = 1\)$
// EXPECT: ^VERIFICATION FAILED$
