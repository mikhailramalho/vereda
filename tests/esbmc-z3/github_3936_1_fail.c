/* Regression test for https://github.com/esbmc/esbmc/issues/3936
 * Variant: quantifier invariant first, plain invariant second — but the
 * assertion inside the loop is genuinely false.  Ensures the fix does not
 * suppress real failures.
 *
 * VERIFICATION FAILED is expected.
 */
int main()
{
  int idx_v0;
  int idx;

  __ESBMC_loop_invariant(
    __ESBMC_forall(&idx_v0,
      !(0 <= idx_v0) || !(idx_v0 < idx) || (idx_v0 < 3)));
  __ESBMC_loop_invariant(0 <= idx);

  for (idx = 0; idx < 3; ++idx)
  {
    __VERIFIER_assert(idx > 3); // this is genuinely false
  }

  return 0;
}
// Imported from ESBMC regression/z3/github_3936_1_fail
// Needs: __ESBMC_forall, __ESBMC_loop_invariant
// Needs: --loop-invariant --no-standard-checks
// RUN: --loop-invariant --no-standard-checks --z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
