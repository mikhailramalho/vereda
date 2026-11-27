#include <stdlib.h>

struct S
{
  int x;
  char a[];
};

int main()
{
  struct S *p=malloc(sizeof(struct S)+10);
  __VERIFIER_assume(p);
  
  p->x=1;
  p->a[0]=3;
  p->a[9]=3;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Structs6
// EXPECT: ^VERIFICATION SUCCESSFUL$
