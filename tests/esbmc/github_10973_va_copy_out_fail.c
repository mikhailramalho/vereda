// Copying p out with va_copy leaves p itself denoting f's own arguments.
#include <stdarg.h>
#include <stdio.h>

static void f(const char *s, ...)
{
  va_list p, q;
  va_start(p, s);
  va_copy(q, p);
  vprintf(s, p);
  va_end(q);
  va_end(p);
}

int main(void)
{
  char buf[1] = {'a'};
  f("%s", buf);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10973_va_copy_out_fail
// Needs: --printf-check
// RUN: --printf-check
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: printf %s argument is not NUL-terminated within its object
// Vereda: verdict none
