int main(void)
{
  int a[5] = {1, 2};
  return a[3];
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array_initlist
// RUN: --ub-uninit-check --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
