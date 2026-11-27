#include <stdbool.h>

/* Issue #10997: failing counterpart; the invariant is false, and the
 * base case must report it. */
bool f_true(int arg)
{
  return true;
}

bool f_false(int arg)
{
  return false;
}

int main()
{
  int var1, var2;
  int idx;
  __ESBMC_loop_invariant(f_false(var1) && f_true(var2));
  for (idx = 0; idx < 2; ++idx)
  {
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10997_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --no-standard-checks
// RUN: --loop-invariant-check --no-standard-checks --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED .*loop invariant base case$
