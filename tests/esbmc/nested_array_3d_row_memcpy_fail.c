#include <string.h>
int __VERIFIER_nondet_int(void);

int main(void)
{
  int a[2][2][2];
  int b[2];
  int i = __VERIFIER_nondet_int();
  int j = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  __VERIFIER_assume(j >= 0 && j < 2);

  a[0][0][0] = 1;
  a[0][0][1] = 1;
  a[0][1][0] = 1;
  a[0][1][1] = 1;
  a[1][0][0] = 1;
  a[1][0][1] = 1;
  a[1][1][0] = 1;
  a[1][1][1] = 1;
  b[0] = 5;
  b[1] = 6;

  memcpy(a[1][1], b, sizeof b);

  __VERIFIER_assert(
    a[i][1][j] == (i == 1 ? (j == 0 ? 6 : 5) : 1)
); // the innermost grafted row must not read back with its elements swapped
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_3d_row_memcpy_fail
// EXPECT: ^VERIFICATION FAILED$
