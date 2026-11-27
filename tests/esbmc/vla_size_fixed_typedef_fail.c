#include <assert.h>
int main(void)
{
  int n = 3;
  typedef int row[n];
  n = 5;
  row x;
  assert(sizeof(x) == 5 * sizeof(int));
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_size_fixed_typedef_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
