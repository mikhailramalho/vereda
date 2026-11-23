#include <time.h>
#include <assert.h>

int main()
{
  struct timespec ts;
  int result = timespec_get(&ts, TIME_UTC);
  if (result == TIME_UTC)
  {
    assert(ts.tv_nsec >= 0);
    assert(ts.tv_nsec <= 999999999L);
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_timespec_get_ranges
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
