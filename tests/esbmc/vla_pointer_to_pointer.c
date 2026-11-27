#include <assert.h>
int __VERIFIER_nondet_int(void);
int main(void)
{
  int m = __VERIFIER_nondet_int();
  __VERIFIER_assume(m >= 2 && m <= 4);
  int a[3][m];
  a[1][1] = 7;
  a[2][m - 1] = 9;
  int (*p)[m] = a;
  int (**pp)[m] = &p;
  assert((*pp)[1][1] == 7);
  *pp = a + 2;
  assert((**pp)[m - 1] == 9);
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_pointer_to_pointer
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
