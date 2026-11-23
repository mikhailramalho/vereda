#include <stdarg.h>

static int through_copy(int n, ...)
{
  va_list ap, cp;
  va_start(ap, n);
  va_copy(cp, ap);
  int a = va_arg(cp, int);
  va_end(cp);
  va_end(ap);
  return a;
}

int main()
{
  __VERIFIER_assert(through_copy(1, 7) == 7); // va_arg reads through the copy
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_va_copy_binds_by_reference
// Needs: --clang-c-irep2-adjust-only --symbol-table-only
// RUN: --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^__builtin_va_copy\(&cp(\[0\])?, &ap(\[0\])?[,)]
// Vereda: verdict none
