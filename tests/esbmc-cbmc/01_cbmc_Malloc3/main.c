
int main() {
  int *p;
  unsigned int n;

  p=malloc(sizeof(int)*10);

  free(p);
  
  // bad!
  p[1]=1;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc3
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
