#include <assert.h>

int main()
{
  unsigned _BitInt(40) um = 0;
  um = ~um;
  um = um >> 1;
  signed _BitInt(40) m = (signed _BitInt(40))um;
  signed _BitInt(40) r = m - 1 + 1;
  assert(r == m);
  unsigned _BitInt(40) wrap = um + um + 2;
  assert(wrap == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_7363_bitint40
// Needs: REQUIRES bitint
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
