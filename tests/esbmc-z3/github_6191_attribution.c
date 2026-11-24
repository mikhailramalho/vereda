int main()
{
  int a[12], b[12], i, j;
  __VERIFIER_assume(__ESBMC_forall(&j, !(0 <= j && j < 12) || b[j] == 7));
  __VERIFIER_assert(__ESBMC_forall(&j, !(0 <= j && j < 12) || b[j] == 7)); // holds
  __VERIFIER_assert(
    __ESBMC_forall(&i, !(0 <= i && i < 12) || a[i] != a[0])); // genuine
  return 0;
}
// Imported from ESBMC regression/z3/github_6191_attribution
// Needs: --no-standard-checks
// RUN: --z3 --unwind 13 --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: \A(?![\s\S]*Can't get boolean value)[\s\S]*\Z
// Vereda: verdict none
