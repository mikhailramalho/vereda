/* Regression test for https://github.com/esbmc/esbmc/issues/3936
 * False plain invariant (inductive step): idx starts at 0 so the base
 * case holds, but after the first iteration idx becomes 1, breaking the
 * invariant idx < 1.  The inductive-step ASSERT should fail.
 *
 * VERIFICATION FAILED is expected.
 */
int main()
{
  int idx_v0;
  int idx;

  __ESBMC_loop_invariant(idx < 1); /* true at base (idx=0), false after first increment */
  __ESBMC_loop_invariant(
    __ESBMC_forall(&idx_v0,
      !(0 <= idx_v0) || !(idx_v0 < idx) || (idx_v0 < 3)));

  for (idx = 0; idx < 3; ++idx)
  {
    __VERIFIER_assert(0 <= idx); // bounds
  }

  return 0;
}
// Imported from ESBMC regression/z3/github_3936_inv_inductive_fail
// Needs: __ESBMC_forall, __ESBMC_loop_invariant
// Needs: --loop-invariant --no-standard-checks
// RUN: --loop-invariant --multi-property --no-standard-checks --z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.2\]\s+line\s+18\s+loop invariant inductive step$
// EXPECT: ^VERIFICATION FAILED$
