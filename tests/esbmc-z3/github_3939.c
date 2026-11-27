// BAD: idx declared before max = vec[0]; should still PASS (issue #3939)
int main()
{
  int idx_v0;
  int vec[3];
  int max;
  int idx;
  max = vec[0];
  __ESBMC_loop_invariant(
    __ESBMC_forall(
      &idx_v0,
      !(0 <= idx_v0) ||
      !(idx_v0 < 3) ||
      !(idx_v0 < idx) ||
      (vec[idx_v0] <= max)));
  __ESBMC_loop_invariant(1 <= idx);
  for (idx = 1; idx < 3; ++idx)
  {
    if (max < vec[idx])
      max = vec[idx];
  }
  return 0;
}
// Imported from ESBMC regression/z3/github_3939
// Needs: __ESBMC_forall, __ESBMC_loop_invariant
// Needs: --loop-invariant --no-standard-checks
// RUN: --loop-invariant --no-standard-checks --z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
