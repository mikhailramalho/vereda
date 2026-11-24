#include <time.h>
#include <assert.h>

int main(void)
{
  struct tm t = {0};
  t.tm_year = 102;
  t.tm_mon = 21;
  t.tm_mday = 9;
  t.tm_wday = 5;

  timegm(&t);

  /* tm_wday is recomputed from the normalised date, so the value written
     before the call must not survive it. */
  assert(t.tm_wday == 5);

  return 0;
}
// Imported from ESBMC regression/esbmc/github_7141-timegm_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: FUTURE test passed unexpectedly; promote it to CORE
// MODE: CORE
