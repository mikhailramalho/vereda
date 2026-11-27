#include <assert.h>
#include <stdint.h>
#include <string.h>

int main()
{
  uint64_t u = 0x4000000000000000;
  double d;
  memcpy(&d, &u, sizeof d);
  assert(d == 2.0);
  assert(d < 2.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/memcpy_int_to_float_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
