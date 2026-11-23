int main()
{
  int a, b, c;

  __VERIFIER_assert(a + b == b + a); // A1
  __VERIFIER_assert(a + b == a + c); // A2
}
// Imported from ESBMC regression/esbmc/github_7023
// Needs: --result-only
// RUN: --result-only
// MODE: FUTURE
// EXPECT: ^\*\* Results:$
// EXPECT: ^ +PASSED .*line +5 +A1$
// EXPECT: ^ +FAILED .*line +6 +A2$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
