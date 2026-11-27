// %s reads the string a va_list passes on, as it does a direct argument.
#include <stdarg.h>
#include <stdio.h>

static void w(const char *s, ...)
{
  va_list p;
  va_start(p, s);
  vprintf(s, p);
  va_end(p);
}

int main(void)
{
  w("%s\n", (char *)0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10973_null_s_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: dereference failure: NULL pointer
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
