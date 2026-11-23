int main()
{
  int a, b;

  __VERIFIER_assert(a + b == b + a); // \t
  __VERIFIER_assert(a == 1); // P2
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7023-blank-desc
// Needs: --result-only
// RUN: --result-only
// MODE: FUTURE
// EXPECT: ^  PASSED +\[main\.assertion\.1\]  line 5[^\t\n]*$
// EXPECT: ^  FAILED +\[main\.assertion\.2\]  line 6  P2$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
