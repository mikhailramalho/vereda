extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x;
  if (__VERIFIER_nondet_int())
    x = 1;
  return x;
}
// Imported from ESBMC regression/esbmc/cwe_uninit_var
// RUN: --uninitialised-vars-check --incremental-bmc
// Vereda: unmet EXPECT: ^  CWE: CWE-457$ (Vereda prints no CWE line)
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  use of uninitialized variable: x$
// EXPECT: ^  CWE: CWE-457$
// Vereda: verdict FAILED
