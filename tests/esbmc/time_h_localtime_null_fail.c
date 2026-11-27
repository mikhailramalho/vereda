#include <time.h>

int main()
{
  struct tm *result = localtime((void *)0);
  (void)result;
  return 0;
}
// Imported from ESBMC regression/esbmc/time_h_localtime_null_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
