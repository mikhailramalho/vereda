#include <stdlib.h>

void my_free(int *q)
{
  free(q);
}

int main()
{
  int *p=malloc(sizeof(int));
  __VERIFIER_assume(p);
  
  *p=2;
  free(p);
//  my_free(p);

//  *p=3;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Free4
// EXPECT: ^VERIFICATION SUCCESSFUL$
