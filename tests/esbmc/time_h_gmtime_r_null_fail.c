#include <time.h>

int main()
{
  struct tm buf;
  struct tm *result = gmtime_r((void *)0, &buf);
  (void)result;
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_gmtime_r_null_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
