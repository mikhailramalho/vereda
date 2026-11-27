#include <time.h>
#include <assert.h>

int main()
{
  /* Call tzset to initialize timezone variables */
  tzset();

  /* daylight should be 0 or 1 */
  assert(daylight == 0 || daylight == 1);

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/time_tzset
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
