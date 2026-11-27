#include <assert.h>
int main(void)
{
  int m = 3, a[m];
  assert(sizeof(a) == 2 * sizeof(int));
  return 0;
}
// Imported from ESBMC regression/esbmc/vla_size_same_declaration_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
