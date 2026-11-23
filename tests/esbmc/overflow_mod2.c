#include <limits.h>

int main()
{
  unsigned int ui = 1, uj = 2;
  unsigned int mod_wrap = ui % uj;
  return 0;
}
// Imported from ESBMC regression/esbmc/overflow_mod2
// RUN: --unsigned-overflow-check --solver z3
// EXPECT: ^VERIFICATION SUCCESSFUL$
