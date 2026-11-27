int __VERIFIER_nondet_int(void);
int main(void)
{
  int m = __VERIFIER_nondet_int();
  __VERIFIER_assume(m >= 3 && m <= 5);
  int a[2][m][3];
  __VERIFIER_assert(sizeof(a) > 72); // declaration only
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_middle_dim_decl_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
