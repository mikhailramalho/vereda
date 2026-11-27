#include <time.h>

int main()
{
  struct tm *result = gmtime((void *)0);
  (void)result;
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_gmtime_null_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
