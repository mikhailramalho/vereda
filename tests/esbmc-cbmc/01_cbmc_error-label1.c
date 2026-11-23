int main()
{
  int i;

  // this should fail  
  goto ERROR;

  return 0;

  ERROR:;
}
// Imported from ESBMC regression/cbmc/01_cbmc_error-label1
// Needs: --error-label ERROR
// RUN: --error-label ERROR
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
