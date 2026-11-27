int main()
{
  int i;

  // this should fail  
  goto ERROR;

  return 0;

  ERROR:;
}
// Imported from ESBMC regression/cbmc/01_cbmc_error-label1
// RUN: --error-label ERROR
// EXPECT: ^VERIFICATION FAILED$
