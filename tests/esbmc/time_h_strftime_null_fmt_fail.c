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
  t.tm_wday = 1;
  t.tm_yday = 0;
  t.tm_isdst = 0;
  char buf[64];
  size_t result = strftime(buf, sizeof(buf), (void *)0, &t);
  (void)result;
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_strftime_null_fmt_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
