#include <string.h>
int __VERIFIER_nondet_int(void);

int main(void)
{
  int a[2][2];
  int b[2];
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);

  a[0][0] = 9;
  a[0][1] = 9;
  a[1][0] = 1;
  a[1][1] = 2;
  b[0] = 5;
  b[1] = 4;

  memcpy((char *)a + sizeof a[0], b, sizeof a[0]);

  __VERIFIER_assert(a[1][i] == 5 || a[1][i] == 4); // the grafted row reads back
  __VERIFIER_assert(a[0][i] == 9); // the untouched row is unchanged
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_row_memcpy
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
