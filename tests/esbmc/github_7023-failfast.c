int main()
{
  int a, b, c;

  __VERIFIER_assert(a == 1); // P1
  __VERIFIER_assert(b == 2); // P2
  __VERIFIER_assert(c == 3); // P3
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7023-failfast
// Needs: --multi-fail-fast 0
// RUN: --multi-property --multi-fail-fast 0 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// EXPECT: \A(?![\s\S]*PASSED)[\s\S]*\Z
// Vereda: verdict none
