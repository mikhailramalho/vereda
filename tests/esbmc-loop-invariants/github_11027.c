/* Regression: GitHub #11027 -- x is not written in the loop, so the first
 * iteration from the entry state violates the assertion: a real failure, not
 * an invariant too weak to prove it, and its trace names the property. */
int main()
{
  int x = 0;
  int idx;
  __ESBMC_loop_invariant(x == 0);
  for (idx = 0; idx < 10; ++idx)
  {
    __VERIFIER_assert(x == 1); // top: x==1
  }
}
// Imported from ESBMC regression/loop-invariants/github_11027
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant-check --cex-output trace.cex
// RUN: --loop-invariant-check --multi-property --cex-output trace.cex --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
