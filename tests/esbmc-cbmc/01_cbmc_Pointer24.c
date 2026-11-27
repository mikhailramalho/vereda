char a[100];

int main()
{
  char *p, *q;
  
  q=p;
  
  __VERIFIER_assume(!__ESBMC_same_object(p, 0));
  
  p++;
  
  assert(!__ESBMC_same_object(p, 0));
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer24
// Needs: __ESBMC_same_object
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
