#include <time.h>
#include <assert.h>

int main()
{
  assert(CLOCKS_PER_SEC > 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_clocks_per_sec
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
