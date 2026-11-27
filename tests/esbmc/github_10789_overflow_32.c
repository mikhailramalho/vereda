/* #10789: 2038-01-19 03:14:08 UTC does not fit a 32-bit time_t, so timegm
   fails and leaves the date alone. */
#include <assert.h>
#include <time.h>

int main(void)
{
  struct tm t = {0};
  t.tm_year = 138;
  t.tm_mday = 19;
  t.tm_hour = 3;
  t.tm_min = 14;
  t.tm_sec = 8;
  t.tm_isdst = 1;

  assert(timegm(&t) == -1);
  assert(t.tm_sec == 8 && t.tm_mday == 19 && t.tm_isdst == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10789_overflow_32
// RUN: -m32 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
