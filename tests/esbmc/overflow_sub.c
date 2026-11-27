#include <stdlib.h>

int main()
{
  unsigned int uc = 0, ud = 1;
  unsigned int sub_wrap = uc - ud;
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_sub
// RUN: --ub-signed-overflow-check --unsigned-overflow-check --ub-shift-check --solver z3 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
