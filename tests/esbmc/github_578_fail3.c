#include <stdint.h>
#include <assert.h>
int main()
{
  uint64_t x = 2;
  x <<= 63;
  x <<= 63;
  assert(x == 1ULL << 63);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_578_fail3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
