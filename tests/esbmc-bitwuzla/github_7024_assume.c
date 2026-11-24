int main()
{
  int a[10];
  int x;

  x = 11;

  __VERIFIER_assume(
    __ESBMC_forall(&x, !(0 <= x && x < 10) || a[x] == 0));

  __VERIFIER_assert(a[3] == 0); // assumed forall constrains a[3]
}
// Imported from ESBMC regression/bitwuzla/github_7024_assume
// RUN: --solver bitwuzla
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
