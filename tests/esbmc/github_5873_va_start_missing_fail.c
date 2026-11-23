#include <stdarg.h>

/* va_arg through a va_list never initialised by va_start is undefined
 * behaviour and must be flagged, not silently resolved. */
int sum(int count, ...)
{
  va_list ap;
  /* va_start(ap, count); -- deliberately omitted */
  int total = 0;
  for (int i = 0; i < count; i++)
    total += va_arg(ap, int);
  return total;
}

int main()
{
  int r = sum(2, 3, 4);
  __VERIFIER_assert(r == 7); // unreachable under correct varargs semantics
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5873_va_start_missing_fail
// RUN: --entry main
// EXPECT: missing va_start: va_arg on an uninitialised va_list
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: missing va_start: va_arg on an uninitialised va_list
// MODE: FUTURE
// Vereda: verdict UNKNOWN
