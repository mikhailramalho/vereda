#include <string.h>
int __VERIFIER_nondet_int(void);

int main(void)
{
  int a[2][2][2];
  int b[2][2];
  int i = __VERIFIER_nondet_int();
  int j = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 2);
  __VERIFIER_assume(j >= 0 && j < 2);

  a[1][0][0] = 1;
  a[1][0][1] = 1;
  a[1][1][0] = 1;
  a[1][1][1] = 1;
  b[0][0] = 5;
  b[0][1] = 6;
  b[1][0] = 7;
  b[1][1] = 8;

  memcpy((char *)a + sizeof a[0], b, sizeof a[0]);

  __VERIFIER_assert(
    a[1][i][j] >= 6 && a[1][i][j] <= 8); // the plane's first element was grafted too
  return 0;
}
// Imported from ESBMC regression/esbmc/nested_array_plane_memcpy_fail
// EXPECT: ^\s+the plane.s first element was grafted too$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^\s+the plane.s first element was grafted too$
// MODE: FUTURE
// Vereda: verdict FAILED
