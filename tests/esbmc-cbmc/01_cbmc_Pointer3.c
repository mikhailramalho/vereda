int main()
{
  typedef int *intptr;
  intptr a[10];
  int x, y;
  
  unsigned i;
  __VERIFIER_assume(i<10);

  a[i]=&x;
  a[5]=&y;
  
  assert(*(a[i])==x);  
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer3
// EXPECT: ^VERIFICATION FAILED$
