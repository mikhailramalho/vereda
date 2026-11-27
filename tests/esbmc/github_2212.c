#include <assert.h>

int main()
{
  _Float16 a = 1.5;
  _Float16 b = 2.5;
  _Float16 result = a + b;

  assert(result == 4.0);

  return 0;
}
// Imported from ESBMC regression/esbmc/github_2212
// RUN: --no-incremental-bmc
// MODE: THOROUGH
// EXPECT: ^VERIFICATION SUCCESSFUL$
