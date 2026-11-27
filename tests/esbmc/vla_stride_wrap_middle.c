int __VERIFIER_nondet_int(void);
int main(void)
{
  int m = __VERIFIER_nondet_int();
  __VERIFIER_assume(m == 715827883); /* 6 * m == 2^32 + 2 */
  int i = __VERIFIER_nondet_int(), k = __VERIFIER_nondet_int();
  __VERIFIER_assume(i == 1 && k == 2);
  int a[2][2][m][3];
  a[0][0][0][k] = 1;
  a[i][0][0][0] = 2;
  __VERIFIER_assert(a[0][0][0][k] == 1); // distinct elements
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_stride_wrap_middle
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
