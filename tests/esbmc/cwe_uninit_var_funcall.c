extern int foo(void);

int main(void)
{
  int x;
  x = foo();
  return x;
}
// Imported from ESBMC regression/esbmc/cwe_uninit_var_funcall
// RUN: --ub-uninit-check --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
