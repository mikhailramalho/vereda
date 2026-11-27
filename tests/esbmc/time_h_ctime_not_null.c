#include <time.h>
#include <assert.h>

int main()
{
  time_t t = 1000000;
  char *result = ctime(&t);
  assert(result != (void *)0);
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_ctime_not_null
// EXPECT: ^VERIFICATION SUCCESSFUL$
