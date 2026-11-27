#include <stdarg.h>

static void f(int n, ...)
{
  va_list p;
  (void)va_arg(p, int);
}

int main(void)
{
  f(1, 2);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10973_va_arg_unstarted_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: missing va_start: va_arg on an uninitialised va_list
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
