#include <assert.h>
#include <stdlib.h>

int main()
{
  int *p = NULL;
  assert(__ESBMC_r_ok(p, 0));
  return 0;
}
// Imported from ESBMC regression/esbmc/r_ok24
// Needs: __ESBMC_r_ok
// RUN: --ub-pointer-check=false
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
