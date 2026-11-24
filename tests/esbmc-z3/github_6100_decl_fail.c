int helper(int a)
{
  int t = a * 2;
  return t + 1;
}

int main()
{
  int x;
  __VERIFIER_assert(__ESBMC_forall(&x, helper(x) == 2 * x)); // always odd
}
// Imported from ESBMC regression/z3/github_6100_decl_fail
// Needs: __ESBMC_forall
// RUN: --solver z3
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
