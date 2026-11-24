/* Regression: GitHub #7478 -- declining the loop must not drop the base case.
 * `cnt` is 4 on entry, so the annotated invariant is false there and has to be
 * reported even though the havoc cannot cover the loop's pointer write. */
#include <assert.h>

int main()
{
  int cnt = 4;
  int *p = &cnt;
  __ESBMC_loop_invariant(cnt < 0);
  while (*p)
  {
    (*p)--;
  }
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7478_declined_base_case_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.2\]\s+line\s+11\s+loop invariant base case$
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.3\]\s+line\s+11\s+loop invariant inductive step$
// EXPECT: ^VERIFICATION FAILED$
