int main()
{
  int x;

  x = 5;

  __VERIFIER_assert(__ESBMC_exists(&x, x == 13)); // exists eq
}
// Imported from ESBMC regression/bitwuzla/github_7024_exists
// Needs: __ESBMC_exists
// RUN: --solver bitwuzla
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
