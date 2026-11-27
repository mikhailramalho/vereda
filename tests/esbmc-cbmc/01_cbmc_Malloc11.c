
int main()
{
  int *p;
  
  p=malloc(100*sizeof(int));

  // buffer overflow
  p[100]=1;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc11
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
