int impure(int a)
{
  int s = 0;
  for (int i = 0; i < a; i++)
    s += i;
  return s;
}

int main()
{
  int var;
  __VERIFIER_assert(__ESBMC_exists(&var, impure(var) == 0)); // e
}
// Imported from ESBMC regression/z3/github_6100_noninline
// Needs: __ESBMC_exists
// RUN: --solver z3
// EXPECT: cannot model call to .* on a quantified variable inside __ESBMC_forall/__ESBMC_exists
// Vereda: over 300 s in CI
// MODE: KNOWNBUG
