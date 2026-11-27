/* The model violating `bounds` only through the havoc (an abstraction-only
   finding) must not hide the failing inductive step of idx < 1 (#10641). */
int main()
{
  int idx_v0;
  int idx;

  __ESBMC_loop_invariant(idx < 1);
  __ESBMC_loop_invariant(
    __ESBMC_forall(&idx_v0, !(0 <= idx_v0) || !(idx_v0 < idx) || (idx_v0 < 3)));

  for (idx = 0; idx < 3; ++idx)
    __VERIFIER_assert(0 <= idx); // bounds

  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_10641_abstraction_hides_inductive_fail
// Needs: __ESBMC_forall, __ESBMC_loop_invariant
// Needs: --loop-invariant --no-standard-checks
// RUN: --loop-invariant --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s+FAILED\s+\[main\.assertion\.2\]\s+line\s+12\s+loop invariant inductive step$
