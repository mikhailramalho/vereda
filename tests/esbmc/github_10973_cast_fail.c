// A cast on the va_list operand does not hide which va_list it is.
#include <stdarg.h>
#include <stdio.h>

static void f(const char *s, ...)
{
  va_list p;
  va_start(p, s);
  vprintf(s, (void *)p);
  va_end(p);
}

int main(void)
{
  char buf[1] = {'a'};
  f("%s", buf);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10973_cast_fail
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: printf %s argument is not NUL-terminated within its object
// Vereda: verdict none
