/* Regression: GitHub #7478 -- the write is the return of a call, `*p = dec(*p)`,
 * not an ASSIGN.  Its target is still a dereference the havoc cannot cover. */
#include <assert.h>

static int dec(int c)
{
  return c - 1;
}

int main()
{
  int cnt = 4;
  int *p = &cnt;
  __ESBMC_loop_invariant(cnt >= 0);
  while (*p)
  {
    *p = dec(*p);
  }
  assert(0);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7478_call_ret_fail
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property
// MODE: FUTURE
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.3\]\s+line\s+15\s+loop invariant inductive step$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
