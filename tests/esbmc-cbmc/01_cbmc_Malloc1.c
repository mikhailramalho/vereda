int main() {
  int *p;
  unsigned o;
  unsigned int n=__VERIFIER_nondet_uint();
  
  __VERIFIER_assume(n>=1);
  __VERIFIER_assume(n<10000000);

  p=malloc(sizeof(int)*n);
  __VERIFIER_assume(p);
  
  o=n-1;

  p[o]=0;

  free(p);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc1
// EXPECT: ^VERIFICATION SUCCESSFUL$
