int main()
{
  int **p;
  int *q;
  int i;
  q=&i;
  p=&q;
 
 
  **p=1;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Pointer17
// EXPECT: ^VERIFICATION SUCCESSFUL$
