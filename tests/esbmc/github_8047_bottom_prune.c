// #8047: the else-branch assume empties the domain; the merge must replace
// that bottom with the other path's domain, so the claim is still discharged.
int main()
{
  int x;
  __VERIFIER_assume(x >= 0 && x <= 10);
  if (x == 5)
    ;
  else
    __VERIFIER_assume(x == 20);
  __VERIFIER_assert(x <= 10); // x at most 10
}
// Imported from ESBMC regression/esbmc/github_8047_bottom_prune
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
