/* Regression: GitHub #7494 -- the verification branch copied no body for a
 * do-while, so ASSUME(INV) was followed straight by ASSERT(INV) and any
 * invariant passed.  `x <= 2` is false once the third iteration runs. */
#include <assert.h>

int main()
{
  int x = 0;
  __ESBMC_loop_invariant(x <= 2);
  do
  {
    x++;
  } while (x < 5);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7494_inv_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant
// RUN: --loop-invariant --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.1\]\s+line\s+10\s+loop invariant inductive step$
// EXPECT: ^VERIFICATION FAILED$
