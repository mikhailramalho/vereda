#include <stdint.h>
#include <assert.h>
int main()
{
  int32_t x = 2;
  x <<= 31;
  x <<= 31;
  assert(x == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_578_fail4
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
