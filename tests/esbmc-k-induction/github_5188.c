/* Regression for issue #5188: under --k-induction --overflow-check ESBMC used
 * to SIGABRT ("Z3 error invalid argument encountered") while building the error
 * trace for this signed-overflow counterexample, instead of reporting the
 * (genuine) overflow. */
int g(int a, int b)
{
  int x = a, prod = 0;
  while (x >= 0)
  {
    prod = prod + b; /* signed overflow reachable in the inductive step */
    x--;
  }
  return prod;
}
// Imported from ESBMC regression/k-induction/github_5188
// Needs: --k-induction
// RUN: --function g --k-induction --overflow-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: arithmetic overflow on add
// Vereda: verdict none
