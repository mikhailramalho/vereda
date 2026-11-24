/* --multi-property --loop-invariant with an INCORRECT invariant.
 *
 * The invariant "res == (idx*(idx+1))/17" is wrong (should be /2).
 * With --multi-property the engine continues past the k=1 inductive-step
 * failure to check remaining properties.  The final verdict must still be
 * VERIFICATION FAILED, not VERIFICATION SUCCESSFUL.
 * Regression test for GitHub issue #3818.
 */
#include <assert.h>

unsigned int sum_loop(unsigned int n)
{
  unsigned int res = 0;
  unsigned int idx = 0;
  __VERIFIER_assume(n < 10);
  __ESBMC_loop_invariant(n < 10);
  __ESBMC_loop_invariant(res == (idx * (idx + 1)) / 17); /* WRONG: should be /2 */
  __ESBMC_loop_invariant(idx <= n);
  while(idx < n)
  {
    ++idx;
    res += idx;
  }
  return res;
}

int main(void)
{
  unsigned int n;
  __VERIFIER_assume(n < 10);
  unsigned int r = sum_loop(n);
  assert(r == (n * (n + 1)) / 2);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/multi_property_wrong_invariant
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant
// RUN: --loop-invariant --multi-property
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
