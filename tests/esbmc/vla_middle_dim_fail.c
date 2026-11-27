#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int m = __VERIFIER_nondet_int();
  __VERIFIER_assume(m >= 2 && m <= 4);
  int a[2][m][3];
  int i = __VERIFIER_nondet_int(), j = __VERIFIER_nondet_int(), k = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2 && j >= 0 && j < m && k >= 0 && k < 3);
  a[i][j][k] = 7;
  a[1 - i][j][k] = 9;
  assert(a[i][j][k] == 9);
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_middle_dim_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
