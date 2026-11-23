#include <stdarg.h>
#include <assert.h>

double sum(int n, ...)
{
  va_list ap;
  va_start(ap, n);
  int total = 0;
  for (int i = 0; i < n; ++i)
    total += va_arg(ap, int);
  double scale = va_arg(ap, double);
  va_end(ap);
  return total * scale;
}

int main(void)
{
  assert(sum(3, 1, 2, 3, 2.0) == 12.0);
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_va_arg
// Needs: --goto-functions-only --clang-c-irep2-adjust-only
// RUN: --goto-functions-only --clang-c-irep2-adjust-only
// MODE: FUTURE
// EXPECT: DECL signed int return_value\$___ESBMC_va_arg\$1;
// EXPECT: ASSIGN return_value\$___ESBMC_va_arg\$1=va_arg\(ap(\[0\])?\)
// EXPECT: DECL double return_value\$___ESBMC_va_arg\$2;
// EXPECT: ASSIGN return_value\$___ESBMC_va_arg\$2=va_arg\(ap(\[0\])?\)
// Vereda: verdict none
