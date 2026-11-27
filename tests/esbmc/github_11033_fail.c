#include <stdbool.h>

/* Issue #11033: a branching invariant after another one hid the first. */
bool f_true()
{
  return true;
}

int main()
{
  int idx0 = 0;
  int idx1 = 0;
  __ESBMC_loop_invariant(idx0 == idx1 + 1);
  __ESBMC_loop_invariant(f_true() || f_true());
  while (idx0 < 10)
  {
    __VERIFIER_assert(idx0 == idx1); // top: equal
    ++idx0;
    idx1 = idx0;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_11033_fail
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --no-standard-checks
// RUN: --loop-invariant-check --no-standard-checks --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  FAILED .*loop invariant base case
