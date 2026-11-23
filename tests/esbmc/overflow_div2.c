#include <limits.h>

int main()
{
  unsigned int ug = 1, uh = 2;
  unsigned int div_wrap = ug / uh;
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_div2
// RUN: --unsigned-overflow-check --solver z3
// EXPECT: ^VERIFICATION SUCCESSFUL$
