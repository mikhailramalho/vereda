int helper(int a)
{
  int t = a * 2;
  return t + 1;
}

int main()
{
  int x;
  __VERIFIER_assert(
    __ESBMC_forall(&x, helper(x) - 2 * x == 1)); // decl body inlines
}
// Imported from ESBMC regression/z3/github_6100_decl
// Needs: __ESBMC_forall
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
