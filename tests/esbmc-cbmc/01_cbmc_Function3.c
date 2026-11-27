void f(int x);

void g()
{
  f(10);
}

void f(int x)
{
}

int main()
{
  g();
}
// Imported from ESBMC regression/cbmc/01_cbmc_Function3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
