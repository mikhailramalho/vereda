#include <limits.h>

int main()
{
  int g = INT_MIN, h = -1;
  int div_overflow = g / h; // Undefined behavior
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_div
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
