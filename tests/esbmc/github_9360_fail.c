/* #9360: a[i++] has type int[m], a variable length array, so it is evaluated
   and i becomes 1. */
#include <assert.h>

int main(void)
{
  int n = 3, m = 4, i = 0;
  int a[n][m];
  (void)sizeof(a[i++]);
  assert(i == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9360_fail
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
