int main(void)
{
  int a[5] = {1, 2};
  return a[3];
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array_initlist
// Needs: --uninitialised-vars-check
// RUN: --uninitialised-vars-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
