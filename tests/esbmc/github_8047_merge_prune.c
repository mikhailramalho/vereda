// #8047: after joining both paths, x is still in [0, 10], so the claim is
// discharged by the interval domain.
int main()
{
  int x;
  __VERIFIER_assume(x >= 0 && x <= 10);
  if (x == 5)
    ;
  else
    __VERIFIER_assume(x == 7);
  __VERIFIER_assert(x <= 10); // x at most 10
}
// Imported from ESBMC regression/esbmc/github_8047_merge_prune
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
