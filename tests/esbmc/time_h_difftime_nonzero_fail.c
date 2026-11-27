#include <time.h>
#include <assert.h>

int main()
{
  time_t a = time(NULL);
  time_t b = time(NULL);
  double d = difftime(a, b);
  assert(d == 0.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_difftime_nonzero_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
