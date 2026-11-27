void f(char *p)
{
  p[1]=1;
}

int main () 
{
  char dummy[10];
  f(dummy);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer19
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
