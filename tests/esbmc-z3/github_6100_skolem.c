int myabs(int a)
{
  if (a < 0)
    return -a;
  return a;
}

int main()
{
  int x;
  __VERIFIER_assert(
    __ESBMC_forall(&x, x == (-0x7fffffff - 1) || myabs(x) >= 0)
); // abs nonnegative away from INT_MIN
}
// Imported from ESBMC regression/z3/github_6100_skolem
// Needs: __ESBMC_forall
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
