int f(int i)
{
  assert(i!=i+1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_function_option1
// RUN: --entry f
// EXPECT: ^VERIFICATION SUCCESSFUL$
