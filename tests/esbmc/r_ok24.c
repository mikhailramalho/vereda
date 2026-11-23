#include <assert.h>
#include <stdlib.h>

int main()
{
  int *p = NULL;
  assert(__ESBMC_r_ok(p, 0));
  return 0;
}
// Imported from ESBMC regression/esbmc/r_ok24
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
