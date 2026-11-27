#include <limits.h>

int main()
{
  int a = INT_MAX, b = 1;
  int add_overflow = a + b; // Undefined behavior
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_add
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
