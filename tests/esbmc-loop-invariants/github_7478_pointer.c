/* Regression: GitHub #7478 -- the loop writes a scalar through a dereference,
 * which has no named symbol for the havoc to cover.  The schema declines the
 * loop and says so rather than reporting a proof it did not make; the unwinder
 * then discharges the post-loop assertion. */
#include <assert.h>

int main()
{
  int cnt = 4;
  int *p = &cnt;
  __ESBMC_loop_invariant(cnt >= 0);
  while (*p)
  {
    (*p)--;
  }
  assert(cnt == 0);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7478_pointer
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.3\]\s+line\s+12\s+loop invariant inductive step$
// EXPECT: ^VERIFICATION SUCCESSFUL$
