void main()
{
  int i=2;
  
  if(__VERIFIER_nondet_bool())
    i++;

  i/=2;
  assert(i==1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Division_Assignment
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
