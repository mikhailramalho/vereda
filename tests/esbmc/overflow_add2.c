#include <limits.h>

int main()
{
  unsigned int ua = UINT_MAX, ub = 1;
  unsigned int add_wrap = ua + ub;
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_add2
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
