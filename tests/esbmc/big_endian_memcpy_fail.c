#include <assert.h>
#include <string.h>

/* The little-endian reading of big_endian_memcpy. */
int main()
{
  unsigned y = 0x01020304u;
  unsigned x = 0;
  memcpy(&x, &y, 1);
  assert(x == 0x04u);
  return 0;
}
// Imported from ESBMC regression/esbmc/big_endian_memcpy_fail
// Needs: --big-endian
// RUN: --big-endian
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
