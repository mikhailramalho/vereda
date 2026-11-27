#include <assert.h>
#include <stdlib.h>

int main()
{
  int *p = malloc(4);
  assert(__ESBMC_r_ok(p, 1));
  return 0;
}
// Imported from ESBMC regression/esbmc/r_ok25
// Needs: __ESBMC_r_ok
// RUN: --ub-pointer-check=false --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
