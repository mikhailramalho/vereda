int main()
{
  int i, j;

  i = 3;
  j = 7;

  __VERIFIER_assert(
    __ESBMC_forall(&i, !(0 <= i && i < 4) || __ESBMC_exists(&j, j == i))
); // nested exists eq
}
// Imported from ESBMC regression/bitwuzla/github_7024_nested
// Needs: __ESBMC_exists, __ESBMC_forall
// RUN: --solver bitwuzla --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
