
int main()
{
  int *p=malloc(sizeof(int));
  int x;
  int i;
  
  if(i==4711) p=&x;

  // should fail if i==4711
  free(p);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Free2
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
