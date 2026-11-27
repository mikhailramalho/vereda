int main()
{
  int a, b, c;

  __VERIFIER_assert(a + b == b + a); // A1
  __VERIFIER_assert(a + b == a + c); // A2
}
// Imported from ESBMC regression/esbmc/github_7023-multi
// RUN: --multi-property --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^ +PASSED .*line +5 +A1$
// EXPECT: ^ +FAILED .*line +6 +A2$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict FAILED
