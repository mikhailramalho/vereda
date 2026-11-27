#include <time.h>

int main()
{
  int result = timespec_get((void *)0, TIME_UTC);
  (void)result;
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_timespec_get_null_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
