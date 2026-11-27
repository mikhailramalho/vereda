#include <assert.h>
#include <math.h>

int main(void)
{
  assert(nearbyint(2.5) == 2.0);
  assert(nearbyint(3.5) == 4.0);
  assert(nearbyint(-2.5) == -2.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_nearbyint_lowering
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
