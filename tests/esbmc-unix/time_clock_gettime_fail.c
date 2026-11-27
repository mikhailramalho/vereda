#include <time.h>
#include <assert.h>

int main()
{
  struct timespec ts;

  /* Invalid clock ID should fail */
  int ret = clock_gettime(999, &ts);
  assert(ret == 0); /* This should fail - invalid clock ID returns -1 */

  return 0;
}
// Imported from ESBMC regression/esbmc-unix/time_clock_gettime_fail
// EXPECT: ^VERIFICATION FAILED$
