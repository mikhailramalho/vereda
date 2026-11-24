#include <assert.h>
#include <stdlib.h>

int main()
{
  int *p = NULL;
  assert(__ESBMC_r_ok(p, 4)); // invalid dereference
  return 0;
}
// Imported from ESBMC regression/esbmc/r_ok10
// Needs: __ESBMC_r_ok
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
