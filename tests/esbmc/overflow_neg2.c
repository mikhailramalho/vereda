#include <limits.h>

int main()
{
  unsigned int uk = 1;
  unsigned int neg_wrap = -uk;
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_neg2
// RUN: --unsigned-overflow-check --solver z3
// EXPECT: ^VERIFICATION FAILED$
