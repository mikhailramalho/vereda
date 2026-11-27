static int f()
{
  int local=1;
  return local;
}

int f1()
{
  return f();
}

int f2();

int main()
{
  assert(f1()==1);
  assert(f2()==2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Static_Functions1
// RUN: --no-incremental-bmc
// FILES: file2.c
// EXPECT: ^VERIFICATION SUCCESSFUL$
