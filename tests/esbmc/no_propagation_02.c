int main()
{
  int x = 42;
  int y = x + 1;
  __VERIFIER_assert(y == 43); // y is 43
  return 0;
}
// Imported from ESBMC regression/esbmc/no_propagation_02
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
