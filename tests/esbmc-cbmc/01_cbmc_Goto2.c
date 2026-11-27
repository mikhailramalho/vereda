int main() {

  int i, j;

  i=1;

  if(j)
    goto l;

  i=2;
 
 l:; 
 
  assert(i==1 || !j);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Goto2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
