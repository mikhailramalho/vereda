/* A recursive call whose argument is symbolic over more than one value once
 * made the path after the call vacuous, so this false assertion was reported
 * SUCCESSFUL. f(n) is 1 for both admitted values, never 999. */
extern int __VERIFIER_nondet_int(void);

int f(int n)
{
  if (n <= 1)
    return n;
  return f(n - 1);
}

int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n == 2 || n == 3);
  int r = f(n);
  __VERIFIER_assert(r == 999); // r is 1, never 999
  return r;
}
// Imported from ESBMC regression/esbmc/github_5387
// RUN: --unwind 24
// EXPECT: ^VERIFICATION FAILED$
