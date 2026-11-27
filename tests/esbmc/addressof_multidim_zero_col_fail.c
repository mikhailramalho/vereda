// Anti-vacuity twin: &a[1][0] is 3 elements past &a[0][0], not 4, so the
// equality has to be refuted.
#include <assert.h>

int main(void)
{
  int a[2][3];
  int *p = &a[1][0];
  int *q = &a[0][0] + 4;

  assert(p == q);
  return 0;
}
// Imported from ESBMC regression/esbmc/addressof_multidim_zero_col_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
