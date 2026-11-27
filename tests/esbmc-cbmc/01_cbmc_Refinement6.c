int i;

void f()
{
  assert(!(i>100));
}

int main()
{
  int j;
  i=j;
  
  if(i>100) f();
}
// Imported from ESBMC regression/cbmc/01_cbmc_Refinement6
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
