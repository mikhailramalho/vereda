/* A pointee with no static width cannot be havoc'd, so the loop is left to
 * the unwinder rather than keep its pre-loop value. */
#include <assert.h>

int main()
{
  int n = 4;
  int a[n];
  int (*p)[n] = &a;
  a[0] = 0;
  int i = 0;
  __ESBMC_loop_invariant(i >= 0 && i <= 10);
  while (i < 10)
  {
    (*p)[0] = 5;
    i++;
  }
  assert(a[0] == 5);
  return 0;
}
// Imported from ESBMC regression/loop-invariants/vla_pointee_write
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
