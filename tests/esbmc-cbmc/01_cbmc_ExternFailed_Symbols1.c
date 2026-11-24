int main()
{
  int *p, *q, *a, *b;
  
  q=a;
  p=b;
  
  *p=1;
  *q=2;

  // this should work if no pointer checks are enabled  
  assert(*p==1);
  assert(*q==2);
}
// Imported from ESBMC regression/cbmc/01_cbmc_ExternFailed_Symbols1
// RUN: --no-pointer-check
// EXPECT: ^VERIFICATION FAILED$
