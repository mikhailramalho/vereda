// https://github.com/esbmc/esbmc/issues/9351
#include <assert.h>
#include <string.h>
int main(void)
{
  _BitInt(17) a[4];
  memset(a, 0, sizeof a);
  assert(a[3] == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_9351_bitint_memset
// RUN: -m64 --std c23 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
