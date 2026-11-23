#include <assert.h>
#include <stdarg.h>

static int mid(int n, va_list outer, ...)
{
  return va_arg(outer, int);
}

static int g(int n, ...)
{
  va_list ap;
  va_start(ap, n);
  int r = mid(0, ap, 9);
  va_end(ap);
  return r;
}

int main()
{
  assert(g(1, 3) == 9);
  return 0;
}
// Imported from ESBMC regression/esbmc/va_arg_callee_va_list_fail
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict UNKNOWN
