#include <stdlib.h>

void my_free(int *q)
{
  free(q);
}

int main()
{
  int *p=malloc(sizeof(int));
  
  *p=2;
  free(p);
  my_free(p);

//  *p=3;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Free8
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
