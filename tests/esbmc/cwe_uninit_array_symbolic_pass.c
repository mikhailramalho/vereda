extern unsigned __VERIFIER_nondet_uint(void);

int main(void)
{
  int a[4];
  unsigned i = __VERIFIER_nondet_uint();
  unsigned j = __VERIFIER_nondet_uint();
  __VERIFIER_assume(i < 4);
  __VERIFIER_assume(j < 4);
  __VERIFIER_assume(i == j);
  a[i] = 7;
  return a[j];
}
// Imported from ESBMC regression/esbmc/cwe_uninit_array_symbolic_pass
// Needs: --uninitialised-vars-check
// RUN: --uninitialised-vars-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
