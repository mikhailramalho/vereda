#include <assert.h>

int main()
{
  int x = 5;
  __ESBMC_init_object(&x);
  assert(x == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/github_8083_init_object
// Needs: __ESBMC_init_object
// Needs: --interval-analysis
// RUN: --interval-analysis
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
