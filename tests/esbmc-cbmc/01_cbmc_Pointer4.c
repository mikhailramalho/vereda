
int main()
{
  int *p=0;
  int x;

  if(__VERIFIER_nondet_bool())
    p=&x;

  if(p) *p=1;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer4
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
