// --interval-symex-assert: domain proves both claims from x in [5, 10].
int main()
{
  int x;
  __VERIFIER_assume(x >= 5 && x <= 10);
  __VERIFIER_assert(x >= 0); // x must be non-negative
  __VERIFIER_assert(x <= 100); // x must be at most 100
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_symex_assert_prune
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
