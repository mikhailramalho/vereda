// --interval-symex-assert without --multi-property: both claims must be
// pruned by the interval domain (0 VCCs remaining) yet no "PASSED (interval)"
// per-claim log line should be emitted.
int main()
{
  int x;
  __VERIFIER_assume(x >= -3 && x <= 3);
  __VERIFIER_assert(x >= -3); // x lower bound
  __VERIFIER_assert(x <= 3); // x upper bound
  return 0;
}
// Imported from ESBMC regression/esbmc/interval_symex_assert_silent
// Needs: --interval-symex-assert
// RUN: --interval-symex-assert
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
