#include <assert.h>
#include <math.h>

int main()
{
  assert(cbrtl(0x1p1020L) == 0x1p340L);
  assert(cbrtl(-0x1p1020L) == -0x1p340L);
  assert(cbrtl(0x1p-1020L) == 0x1p-340L);
  return 0;
}
// Imported from ESBMC regression/esbmc/cbrtl_double_range
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
