#include <time.h>
#include <assert.h>

int main()
{
  time_t stored;
  time_t returned = time(&stored);
  assert(returned == stored);
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_time_return_eq_stored
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
