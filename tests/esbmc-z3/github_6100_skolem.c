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
// RUN: --solver z3
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
