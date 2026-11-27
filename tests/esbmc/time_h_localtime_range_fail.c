#include <time.h>
#include <assert.h>

int main()
{
  time_t t = time(NULL);
  struct tm *r = localtime(&t);
  if (r != (void *)0)
  {
    assert(r->tm_sec == 0);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_localtime_range_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
