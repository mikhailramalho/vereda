
int main()
{
  int *p, *q;
  
  q=p=malloc(sizeof(int));
  __VERIFIER_assume(q);
  
  *p=2;

  p=malloc(sizeof(int));
  __VERIFIER_assume(p);
  
  *p=3;
  
  //this should fail
  assert(*q==3);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc10
// EXPECT: ^VERIFICATION FAILED$
