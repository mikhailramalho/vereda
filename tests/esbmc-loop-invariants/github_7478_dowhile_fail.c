/* Regression: GitHub #7478 -- the failing half for do-while.  The loop exits
 * with x == 5, so the post-loop assertion is false and must be reported. */
#include <assert.h>

int main()
{
  int x = 0;
  __ESBMC_loop_invariant(x <= 4);
  do
  {
    x++;
  } while (x < 5);
  assert(x == 4);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7478_dowhile_fail
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
