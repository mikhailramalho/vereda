/* Regression test for issue #3815:
 * Doubly-recursive fib_rec with a wrong assertion must FAIL verification.
 * Ensures the fix doesn't suppress real bugs.
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
  __VERIFIER_assume(n < 3);
  /* fib(2) == 1, fib(1) == 1, fib(0) == 0 — none are > 100 */
  __VERIFIER_assert(fib_rec(n) > 100); // fib(n) > 100 is always false for n < 3
  return 0;
}
// Imported from ESBMC regression/esbmc/github_3815_fail
// RUN: --ub-pointer-check=false --ub-div-by-zero-check=false --vla-size-check=false --multi-property --unwind 3 --no-unwinding-assertions
// EXPECT: ^VERIFICATION FAILED$
