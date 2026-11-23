#include <limits.h>

int main()
{
  int e = INT_MAX, f = 3;
  int mul_overflow = e * f; // Undefined behavior
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_mul
// RUN: --overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
