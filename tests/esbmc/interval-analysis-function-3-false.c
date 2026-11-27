int sum(int n)
{
  if (n == 0)
    return 0;
  int tmp = n + sum(n - 1);
  return tmp;
}

int main()
{
  int A = 5;
  int B = sum(A);
  __VERIFIER_assert(B <= 10);   // Should be able to verify!
}
// Imported from ESBMC regression/esbmc/interval-analysis-function-3-false
// Needs: --interval-analysis
// RUN: --interval-analysis --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
