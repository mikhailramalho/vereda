#include <stdarg.h>
#include <stdio.h>

static void put(char *b, const char *f, ...)
{
  va_list ap;
  va_start(ap, f);
  vsprintf(b, f, ap);
  va_end(ap);
}

int main(void)
{
  char b[4];
  put(b, "hello");
  return 0;
}
// Imported from ESBMC regression/esbmc/vsprintf_stores_output_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^\s*FAILED\s+\[put\.array-bounds-violated\.1\]\s+line 8\s+dereference failure: array bounds violated$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
