#include <time.h>
#include <assert.h>

int main()
{
  time_t t = time(NULL);
  assert(t >= 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_time_nonneg_fail
// EXPECT: ^VERIFICATION FAILED$
