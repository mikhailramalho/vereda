/* Regression: GitHub #11027 -- as github_11027, without --multi-property, and
 * entering the loop at idx == 3: the trace shows the value the loop is
 * entered with, not a free havoc value. */
int main()
{
  int x = 0;
  int idx;
  __ESBMC_loop_invariant(x == 0);
  for (idx = 3; idx < 10; ++idx)
  {
    __VERIFIER_assert(x == 1); // top: x==1
  }
}
// Imported from ESBMC regression/loop-invariants/github_11027_single
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check
// RUN: --loop-invariant-check
// MODE: FUTURE
// EXPECT: ^  idx = 3
// EXPECT: ^VERIFICATION FAILED$
