#include <limits.h>

int main()
{
  int a = INT_MAX, b = 1;
  int add_overflow = a + b; // Undefined behavior
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_add
// RUN: --overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
