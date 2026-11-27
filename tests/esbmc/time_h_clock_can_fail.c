#include <time.h>
#include <assert.h>

int main()
{
  clock_t c = clock();
  assert(c != (clock_t)(-1));
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_clock_can_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
