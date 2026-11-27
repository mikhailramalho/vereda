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
// Needs: __ESBMC_forall
// RUN: --solver bitwuzla --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
