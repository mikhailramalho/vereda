#include <assert.h>

int main()
{
  unsigned n = 4;
  unsigned s = 0;
  for (unsigned i = 0; i < n; i++)
    s++;
  assert(s == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/no_simplify_loop_bound
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
