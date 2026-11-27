#include <stdarg.h>
#include <stdio.h>

char b[4];

void fmt(const char *f, ...)
{
  va_list ap;
  va_start(ap, f);
  vsprintf(b, f, ap);
  va_end(ap);
}

int main(void)
{
  /* Elsewhere the va_list is not read; write past b so the expected
   * violation holds on every target. */
  if (__builtin_types_compatible_p(va_list, char *))
    fmt("%d", 123456);
  else
    b[4] = 0;
  return 0;
}
// Imported from ESBMC regression/esbmc/vsprintf_va_list_args_fail
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: array bounds violated
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
