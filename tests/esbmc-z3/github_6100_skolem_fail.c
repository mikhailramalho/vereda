int myabs(int a)
{
  if (a < 0)
    return -a;
  return a;
}

int main()
{
  int x;
  __VERIFIER_assert(__ESBMC_forall(&x, myabs(x) >= 0)); // fails at INT_MIN
}
// Imported from ESBMC regression/z3/github_6100_skolem_fail
// Needs: __ESBMC_forall
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
