int main()
{
  int a[4];
  int i;
  int *p;

  i = 2;
  p = &a[3];

  __VERIFIER_assert(
    __ESBMC_forall(&i, !(0 <= i && i < 4) || &a[i] != p)
); // no element aliases p
}
// Imported from ESBMC regression/bitwuzla/github_7024_addrof
// Needs: __ESBMC_forall
// RUN: --solver bitwuzla
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
