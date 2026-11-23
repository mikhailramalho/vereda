#include <limits.h>

int main()
{
  int k = INT_MIN;
  int neg_overflow = -k; // Undefined behavior
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_neg
// RUN: --overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
