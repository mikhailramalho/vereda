#include <assert.h>
#include <stdint.h>

/* __ESBMC_bitcast(tgt, src) reinterprets *src as the type of *tgt, for equal
 * pointee widths. Pins the example documented in docs/constructs. */
int main()
{
  float f = 1.0f;
  uint32_t u;
  __builtin_memcpy(&u, &f, sizeof *(&u));
  assert(u == 0x3f800000);

  double d = 2.0;
  uint64_t v;
  __builtin_memcpy(&v, &d, sizeof *(&v));
  assert(v == 0x4000000000000000ULL);

  float back;
  __builtin_memcpy(&back, &u, sizeof *(&back));
  assert(back == 1.0f);
  return 0;
}
// Imported from ESBMC regression/esbmc/bitcast_intrinsic
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
