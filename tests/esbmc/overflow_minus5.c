#include <limits.h>

int main()
{
  int c = INT_MIN, d = 1;
  int sub_underflow = c - d; // Undefined behavior
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_minus5
// RUN: --overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
