/* Regression: GitHub #7494 -- pins the shape of the transformation, which the
 * verdict cannot: on the unfixed pass every post-loop claim of a do-while is
 * vacuously true, so a correct program reports SUCCESSFUL either way.
 *
 * Two things have to hold.  The verification branch must contain a copy of the
 * body between ASSUME(INV) and ASSERT(INV), guarded by the loop's continue
 * condition -- without the copy the inductive step is discharged against
 * nothing.  And the k-induction hint must be assumed at the loop head, ahead of
 * the body, not at the tail where the exit test lives. */
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
// Imported from ESBMC regression/loop-invariants/github_7494_verification_branch
// Needs: --loop-invariant --goto-functions-only
// RUN: --loop-invariant --goto-functions-only
// MODE: FUTURE
// EXPECT: ASSUME x <= 4 // loop invariant assume \(verification branch\)\n\s+// .*\n\s+ASSIGN x=x \+ 1;\n\s+// .*\n\s+ASSUME x < 5 // loop continue condition
// EXPECT: ASSUME x <= 4 // loop invariant assume \(k-induction hint\)\n\s+// .*\n\s+ASSIGN x=x \+ 1;
// Vereda: verdict none
