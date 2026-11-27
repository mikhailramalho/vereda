int main()
{
  int a[10];
  int x;
  
  a[1]=1000;
  
  x=*(a+1);
  
  assert(x==1000);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer16
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
