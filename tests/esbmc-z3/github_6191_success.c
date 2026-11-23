int zeroed[12];

int main()
{
  int i;
  __VERIFIER_assert(
    __ESBMC_forall(&i, !(0 <= i && i < 12) || zeroed[i] == 0)
); // true forall
  return 0;
}
// Imported from ESBMC regression/z3/github_6191_success
// Needs: --no-standard-checks
// RUN: --z3 --unwind 13 --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
