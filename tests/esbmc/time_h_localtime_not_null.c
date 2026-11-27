#include <time.h>
#include <assert.h>

int main()
{
  time_t t = 1000000;
  struct tm *result = localtime(&t);
  assert(result != (void *)0);
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_localtime_not_null
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
