// #7974: under --no-propagation the counterexample must come from assume(3 == x).
int main()
{
  int x;
  __VERIFIER_assume(3 == x);
  x++;
  __VERIFIER_assert(x == 5); // x is 5
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7974_fail
// Needs: --no-propagation
// RUN: --no-propagation
// MODE: FUTURE
// EXPECT: ^  x = 4; //
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
