// discussion #6114: an earlier quantifier invariant in a multi-clause cluster
// must be re-evaluated after havoc, not left at its (vacuous) pre-loop value.
#define N 4
int main()
{
  int a[N];
  int i = 0;
  int k;
  __ESBMC_loop_invariant(
    __ESBMC_forall(&k, !(0 <= k) || !(k < i) || a[k] == 0));
  __ESBMC_loop_invariant(__ESBMC_forall(&k, k + 1 == 1 + k));
  __ESBMC_loop_invariant(0 <= i && i <= N);
  while (i < N)
  {
    a[i] = 0;
    ++i;
    __VERIFIER_assert(a[0] == 0); // a0 is zero
  }
}
// Imported from ESBMC regression/loop-invariants/github_6114
// Needs: __ESBMC_forall, __ESBMC_loop_invariant
// Needs: --loop-invariant --no-standard-checks
// RUN: --multi-property --loop-invariant --no-standard-checks --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
