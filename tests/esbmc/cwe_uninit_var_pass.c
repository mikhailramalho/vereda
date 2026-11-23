extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x;
  if (__VERIFIER_nondet_int())
    x = 1;
  else
    x = 2;
  return x;
}
// Imported from ESBMC regression/esbmc/cwe_uninit_var_pass
// Needs: --uninitialised-vars-check
// RUN: --uninitialised-vars-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
