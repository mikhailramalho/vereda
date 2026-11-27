// #8047: the else-branch assume narrowed x in the shared interval domain, and
// the merge did not join the other path back, so x > -7 was pruned as TRUE.
int main()
{
  int x;
  if (x == -128)
    ;
  else
    __VERIFIER_assume(x && x == 7);
  __VERIFIER_assert(x > -7); // x above -7
}
// Imported from ESBMC regression/esbmc/github_8047
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
