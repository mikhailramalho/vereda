unsigned long __VERIFIER_nondet_ulong(void);

int main()
{
  // The nondet counterpart of deref_constant_int_address (#6544).
  int *p = (int *)__VERIFIER_nondet_ulong();
  return *p;
}
// Imported from ESBMC regression/esbmc/deref_nondet_int_address
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
