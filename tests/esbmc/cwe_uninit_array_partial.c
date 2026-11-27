int main(void)
{
  int a[5];
  a[0] = 1;
  return a[1];
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array_partial
// RUN: --ub-uninit-check --incremental-bmc
// Vereda: unmet EXPECT: ^  CWE: CWE-457$ (Vereda prints no CWE line)
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  use of uninitialized variable: a$
// EXPECT: ^  CWE: CWE-457$
// Vereda: verdict FAILED
