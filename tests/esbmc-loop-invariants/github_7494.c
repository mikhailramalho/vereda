/* Regression: GitHub #7494 -- the passing half.  The same loop, asserting what
 * it actually leaves, so the exit path has to survive *and* stay constrained. */
#include <assert.h>

int main()
{
  int x = 0;
  __ESBMC_loop_invariant(x <= 4);
  do
  {
    x++;
  } while (x < 5);
  assert(x == 5);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7494
// Needs: --loop-invariant
// RUN: --loop-invariant
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
