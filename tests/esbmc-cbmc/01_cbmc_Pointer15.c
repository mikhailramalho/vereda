int main()
{
  const void *p;
  int *q;
  int c;

  p=(c?p:0);
  
  p=(int * const)0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer15
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
