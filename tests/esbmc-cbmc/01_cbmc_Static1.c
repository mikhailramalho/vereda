int main() {

  static int a = 0;
  
  assert (a == 0);

  return 0;

}
// Imported from ESBMC regression/cbmc/01_cbmc_Static1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
