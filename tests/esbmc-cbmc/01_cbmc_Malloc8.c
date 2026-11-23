
int main() {
  int *p;
  unsigned int n;

  p=malloc(sizeof(int)*10);
  __VERIFIER_assume(p);

  free(p);

  // bad!
  free(p);  
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc8
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
