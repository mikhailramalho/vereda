int main()
{
  int i;
  
  switch(i)
  {
  case 0:
  case 1:
    assert(i==0 || i==1);
    break;

  case 2:
    assert(i==2);
    break;
    
  default:
    assert(i!=0 && i!=1 && i!=2);
  }
}
// Imported from ESBMC regression/cbmc/01_cbmc_switch1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
