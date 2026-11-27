#include <time.h>

int main()
{
  struct tm t;
  t.tm_year = 2024 - 1900;
  t.tm_mon = 0;
  t.tm_mday = 1;
  t.tm_hour = 0;
  t.tm_min = 0;
  t.tm_sec = 0;
  t.tm_isdst = -1;
  time_t result = mktime(&t);
  (void)result;
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_mktime_callable
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
