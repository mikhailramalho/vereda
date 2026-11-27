#include <limits.h>

int main()
{
  int k = INT_MIN;
  int neg_overflow = -k; // Undefined behavior
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_neg
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
