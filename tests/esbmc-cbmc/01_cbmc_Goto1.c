int main()
{
  int i, j;
  
  if(i)
    goto l;
    
  if(j)
    goto l;
    
  assert(!i && !j);
 
 l:; 
}
// Imported from ESBMC regression/cbmc/01_cbmc_Goto1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
