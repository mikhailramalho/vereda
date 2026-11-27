#include <time.h>
#include <assert.h>

int main()
{
  time_t t = time(NULL);
  double d = difftime(t, t);
  assert(d == 0.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_difftime_self_zero
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
