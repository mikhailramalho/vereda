#include <stdarg.h>
#include <stdarg.h>
#include <assert.h>

int AddNumbers(int n, ...) {
  int Sum = 0;
  va_list ptr;
  va_start(ptr, n);
  for (int i = 0; i < n; i++)
    Sum += va_arg(ptr, int);
  va_end(ptr);
  return Sum;
}

int main() {
  assert(AddNumbers(2, 1, 2) == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_584
// RUN: --unwind 32
// EXPECT: ^VERIFICATION SUCCESSFUL$
