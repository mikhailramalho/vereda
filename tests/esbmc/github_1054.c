int main()
{
  unsigned int a;

  if(a < 10)
  {
    __VERIFIER_assert(a > 10);
  }

  return 0;
}
// Imported from ESBMC regression/esbmc/github_1054
// Needs: --interval-analysis --interval-analysis-simplify
// RUN: --interval-analysis  --interval-analysis-simplify --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
