int x, y;

void f()
{
  int *px=&x;

  x=1;
  px++;
  
  y=*px;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer13
// RUN: --entry f --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
