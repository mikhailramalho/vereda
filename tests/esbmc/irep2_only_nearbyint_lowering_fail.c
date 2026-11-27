#include <assert.h>
#include <math.h>

int main(void)
{
  assert(nearbyint(2.5) == 3.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_nearbyint_lowering_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
