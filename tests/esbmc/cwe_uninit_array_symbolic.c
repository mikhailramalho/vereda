extern unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  int a[4];
  unsigned i = __VERIFIER_nondet_uint();
  unsigned j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4);
  __VERIFIER_assume(j < 4);
  a[i] = 7;
  return a[j];
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array_symbolic
// RUN: --uninitialised-vars-check --incremental-bmc
// Vereda: unmet EXPECT: ^  CWE: CWE-457$ (Vereda prints no CWE line)
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  use of uninitialized variable: a$
// EXPECT: ^  CWE: CWE-457$
// Vereda: verdict FAILED
