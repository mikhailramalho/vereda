int *global;

struct X
{
  int i;
  struct X *n;
};

int main()
{
  struct X *p;
  struct X x;
  int *q;
  
  p=malloc(sizeof(struct X));
  __VERIFIER_assume(p);
  q=&(p->i);
  
  *q=1;
  
  assert(p->i==1);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc21
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
