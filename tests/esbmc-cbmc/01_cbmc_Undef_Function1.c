int f(int i, int j);

int main()
{
  int i;
  i=1;
  i=f(1, 2);
  assert(i==1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Undef_Function1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
