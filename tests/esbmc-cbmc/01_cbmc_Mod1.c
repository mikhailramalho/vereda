int main()
{
  int x, y;
  
  x=y;  
  x%=10;
  assert(x!=-1); // should fail
}
// Imported from ESBMC regression/cbmc/01_cbmc_Mod1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
