int main(void)
{
  int a[4];
  a[5] = 42; /* out-of-bounds write on a stack array */
  return a[0];
}
// Imported from ESBMC regression/esbmc/cwe_stack_overflow
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: array bounds violated: array
// EXPECT: ^  CWE: CWE-121, CWE-125, CWE-129, CWE-131, CWE-193, CWE-787$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
