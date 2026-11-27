/* Regression test for issue #3815:
 * Doubly-recursive fib_rec with a concrete n=2 input.
 * --unwind 3 is sufficient because fib_rec(2) recurses to depth 2.
 */

unsigned int fib_rec(const unsigned int n)
{
  if (n == 0)
    return 0;
  else if (n == 1)
    return 1;
  else
    return fib_rec(n - 1) + fib_rec(n - 2);
}

int main()
{
  unsigned int n;
  __VERIFIER_assume(n == 2);
  __VERIFIER_assert(fib_rec(n) == 1); // fib(2) == 1
  return 0;
}
// Imported from ESBMC regression/esbmc/github_3815_3
// Needs: --no-standard-checks
// RUN: --no-standard-checks --unwind 3 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
