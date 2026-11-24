/* Regression: GitHub #7478 -- a short-circuit guard puts its own GOTO in the
 * loop head block, so the first GOTO after the head is not the loop test. */
#include <assert.h>

int main()
{
  int a = 4, b = 1;
  __ESBMC_loop_invariant(a >= 0);
  while (a-- && b)
  {
  }
  assert(0);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7478_shortcircuit_fail
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
