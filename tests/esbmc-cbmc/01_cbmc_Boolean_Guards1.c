int main() {
  unsigned x;
  int i;
  int a[100];

  // this is guaranteed not to be a buffer overflow  
  if(x<100 && a[x])
  {
    i++;
  }

  __VERIFIER_assume(i<100);

  // this is guaranteed not to be a buffer underflow
  if(i>=0 && a[i])
  {
    i++;
  }
}
// Imported from ESBMC regression/cbmc/01_cbmc_Boolean_Guards1
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
