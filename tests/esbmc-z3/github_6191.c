int main()
{
  int a[12];
  int i;
  __VERIFIER_assert(
    __ESBMC_forall(&i, !(0 <= i && i < 12) || a[i] != a[0])); // false forall
  return 0;
}
// Imported from ESBMC regression/z3/github_6191
// Needs: --no-standard-checks
// RUN: --z3 --unwind 13 --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^Violated property:$
// EXPECT: \A(?![\s\S]*Can't get boolean value)[\s\S]*\Z
// Vereda: verdict none
