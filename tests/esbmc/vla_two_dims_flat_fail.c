#include <assert.h>
int __VERIFIER_nondet_int(void);
long __VERIFIER_nondet_long(void);
int main(void)
{
  int m = __VERIFIER_nondet_int();
  long n = __VERIFIER_nondet_long();
  __VERIFIER_assume(m >= 1 && m <= 3 && n >= 2 && n <= 3);
  int a[2][m][n][3];
  int i = __VERIFIER_nondet_int(), k = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2 && k >= 0 && k < 3);
  a[i][m - 1][1][k] = 5;
  a[i][m - 1][0][k] = 6;
  int *flat = &a[0][0][0][0];
  assert(flat[((i * m + m - 1) * n + 1) * 3 + k] == 6);
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_two_dims_flat_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
