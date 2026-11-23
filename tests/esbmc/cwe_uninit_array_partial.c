int main(void)
{
  int a[5];
  a[0] = 1;
  return a[1];
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array_partial
// Needs: --uninitialised-vars-check
// RUN: --uninitialised-vars-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  use of uninitialized variable: a$
// EXPECT: ^  CWE: CWE-457$
// Vereda: verdict none
