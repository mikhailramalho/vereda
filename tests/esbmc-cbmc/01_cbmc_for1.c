int main() {

  int i=0;
  
  for(;;)
  {
    i++;
    if(i==100) break;
  }
  
  assert(i==100);

  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_for1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
