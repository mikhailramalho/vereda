void f(int x)
{
}

int main()
{
  f(1);
  f(2);
  assert(0);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
