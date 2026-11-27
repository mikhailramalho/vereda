/* Regression: GitHub #7478 -- an array element reached through a pointer.
 * `modifies_pointer_array` reports this shape but only goto_k_induction reads
 * it, so the invariant schema has to recognise it too. */
#include <assert.h>

struct S
{
  int e[4];
};

static void run(struct S *s)
{
  __ESBMC_loop_invariant(s->e[0] >= 0);
  while (s->e[0] > 0)
  {
    s->e[0]--;
  }
  assert(0);
}

int main()
{
  struct S s;
  s.e[0] = 4;
  run(&s);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7478_array_ptr_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property
// MODE: FUTURE
// EXPECT: ^\s+PASSED\s+\[run\.assertion\.3\]\s+line\s+14\s+loop invariant inductive step$
// EXPECT: ^VERIFICATION FAILED$
