int main()
{
  int i, j, k;
  
  i=__VERIFIER_nondet_int();
  k=__VERIFIER_nondet_int();
  
  if(i)
  {
  }
  else
  {
    if(k)
    {
      assert(0);
    }
  }
}
// Imported from ESBMC regression/cbmc/01_cbmc_if2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
