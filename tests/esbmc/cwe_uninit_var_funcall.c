extern int foo(void);

int main(void)
{
  int x;
  x = foo();
  return x;
}
// Imported from ESBMC regression/esbmc/cwe_uninit_var_funcall
// Needs: --uninitialised-vars-check
// RUN: --uninitialised-vars-check --incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
