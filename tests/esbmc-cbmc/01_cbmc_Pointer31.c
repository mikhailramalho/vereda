#include <stdlib.h>

int main()
{
 
  int *foobar = malloc(12);
  __VERIFIER_assume(foobar);
  foobar[0] = 0;
  foobar[2] = 1;
  assert(foobar[0] == 0);
  return 0;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer31
// EXPECT: ^VERIFICATION SUCCESSFUL$
