
int main()
{
  int *p=malloc(sizeof(int));
  __VERIFIER_assume(p);
  int *q=p;
  int i;
  
  if(i==4711) free(q);

  // should fail if i==4711
  *p=1;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Free1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
