int main()
{
  // should fail
  int *p=p;
  *p=0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer23
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
