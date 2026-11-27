int main(void)
{
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10880
// Needs: --help
// RUN: --help --no-incremental-bmc
// MODE: FUTURE
// EXPECT: searched for a violation first
// EXPECT: the run continues with no context
// Vereda: verdict none
