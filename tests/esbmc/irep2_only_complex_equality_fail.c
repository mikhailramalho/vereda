/* #9362 under --clang-c-irep2-adjust-only: a NaN part is unequal to itself. */
#include <assert.h>
#include <math.h>

int main(void)
{
  double _Complex n = NAN;
  assert(n == n);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_complex_equality_fail
// RUN: -m64 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
