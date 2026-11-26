#include <stdarg.h>

static int first(int n, ...)
{
  va_list ap;
  va_start(ap, n);
  int a = va_arg(ap, int);
  va_end(ap);
  return a;
}

int main()
{
  __VERIFIER_assert(first(1, 7) == 7); // va_arg reads the first argument
  return 0;
}
// Imported from ESBMC regression/esbmc/irep2_only_va_binds_by_reference_c23
// Needs: --clang-c-irep2-adjust-only --symbol-table-only
// RUN: --std c23 --clang-c-irep2-adjust-only --symbol-table-only
// MODE: FUTURE
// EXPECT: ^__builtin_c23_va_start\(&ap(\[0\])?[,)]
// Vereda: verdict none
