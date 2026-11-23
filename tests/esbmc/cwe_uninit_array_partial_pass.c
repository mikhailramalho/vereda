int main(void)
{
  int a[5];
  a[0] = 1;
  return a[0];
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array_partial_pass
// Needs: --uninitialised-vars-check
// RUN: --uninitialised-vars-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
