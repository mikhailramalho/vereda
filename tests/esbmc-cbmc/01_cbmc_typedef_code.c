typedef int CODETYPE (int a);

extern CODETYPE func;

int func(int a)
{
  return 1;
}

int main(void)
{
  return func(2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_typedef_code
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
