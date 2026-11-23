int main()
{
  const int ARRAY_SIZE = 10;
  long packet[ARRAY_SIZE];
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5007
// Needs: --k-induction --interval-analysis
// RUN: --k-induction --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
