
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
// Imported from ESBMC regression/incremental-smt/incremental-20
// Needs: --k-induction
// RUN: --k-induction -Wno-error=implicit-function-declaration
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
