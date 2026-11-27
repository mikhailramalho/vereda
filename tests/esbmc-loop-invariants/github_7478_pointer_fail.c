/* Regression: GitHub #7478 -- the failing half.  With the write invisible to
 * the havoc the loop stayed concrete, its exit edge was infeasible and this
 * assertion was reported as passing. */
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
  assert(0);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7478_pointer_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.3\]\s+line\s+11\s+loop invariant inductive step$
// EXPECT: ^VERIFICATION FAILED$
