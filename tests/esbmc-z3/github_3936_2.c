/* Regression test for https://github.com/esbmc/esbmc/issues/3936
 * Sanity check: assertion inside loop with a plain invariant (no quantifier).
 * This case passed before the fix and must continue to pass.
 *
 * VERIFICATION SUCCESSFUL is expected.
 */
int main()
{
  int vec[3];
  int sum = 0;
  int idx_v0;
  int idx;

  __ESBMC_loop_invariant(0 <= idx);

  for (idx = 1; idx < 3; ++idx)
  {
    sum += idx;
    __VERIFIER_assert(0 <= idx); // bounds
  }

  return 0;
}
// Imported from ESBMC regression/z3/github_3936_2
// Needs: --loop-invariant --no-standard-checks
// RUN: --loop-invariant --no-standard-checks --z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
