/* #2074: the nil argument must leave x unconstrained, not fix its interval. */
int f(unsigned char x)
{
  __VERIFIER_assert(x != 200); // x can be 200
  return x;
}
// Imported from ESBMC regression/esbmc/github_2074_fail
// Needs: --interval-analysis
// RUN: --function f --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
