#include <assert.h>
int main(void)
{
  int n = 3;
  int a[2][n];
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < n; j++)
      a[i][j] = 0;
  a[1][0] = 7;
  int (*p)[n] = a;
  n = 5;
  assert(p[1][0] != 7); /* p's row length stays 3 (C11 6.7.6.2p5) */
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_size_fixed_pointer_fail
// RUN: --unwind 4
// EXPECT: ^VERIFICATION FAILED$
