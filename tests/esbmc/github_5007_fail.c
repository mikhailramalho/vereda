int main()
{
  const int ARRAY_SIZE = 10;
  long packet[ARRAY_SIZE];
  packet[20] = 1; // out-of-bounds write
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5007_fail
// Needs: --k-induction --interval-analysis
// RUN: --k-induction --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
