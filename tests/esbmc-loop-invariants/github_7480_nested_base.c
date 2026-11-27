/* Regression: GitHub #7480 -- an inner loop's base case is emitted inside the
 * outer body, downstream of the outer havoc, so the state it is checked from
 * is arbitrary rather than concrete.  `s == i` holds every time the real
 * program enters the inner loop and is still refuted there, so the base case
 * cannot be exempted from the downgrade by name; only the outermost one, which
 * no havoc precedes, is decided against the program. */
#include <assert.h>

int main()
{
  int i = 0, s = 0;
  __ESBMC_loop_invariant(i >= 0);
  while (i < 3)
  {
    int j = 0;
    __ESBMC_loop_invariant(j >= 0 && s == i);
    while (j < 2)
    {
      j++;
    }
    s++;
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7480_nested_base
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\s+PASSED\s+\[main\.assertion\.1\]\s+line\s+13\s+loop invariant base case$
// EXPECT: ^\s+UNKNOWN\s+\[main\.assertion\.3\]\s+line\s+17\s+loop invariant base case \(loop invariant too weak
// EXPECT: ^VERIFICATION UNKNOWN$
