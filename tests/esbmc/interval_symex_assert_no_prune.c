// --interval-symex-assert: domain cannot prove x <= 60 from x in [0, 100];
// SMT must still reach the genuine counterexample at x == 100.
int main()
{
  int x;
  __VERIFIER_assume(x >= 0 && x <= 100);
  __VERIFIER_assert(x <= 60); // x must be at most 60
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_symex_assert_no_prune
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
