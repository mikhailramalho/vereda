#include <time.h>

int main()
{
  struct tm tm;
  /* NULL format should trigger assertion failure */
  char *ret = strptime("2024-01-15", (char *)0, &tm);
  (void)ret;
  return 0;
}
// Imported from ESBMC regression/esbmc-unix/time_strptime_null
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: strptime format must not be NULL
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
