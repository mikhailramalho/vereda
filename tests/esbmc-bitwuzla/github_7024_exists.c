int main()
{
  int x;

  x = 5;

  __VERIFIER_assert(__ESBMC_exists(&x, x == 13)); // exists eq
}
// Imported from ESBMC regression/bitwuzla/github_7024_exists
// RUN: --solver bitwuzla
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
