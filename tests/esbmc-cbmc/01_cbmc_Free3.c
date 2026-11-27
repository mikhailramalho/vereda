
int main() {
  int *p;
  unsigned int n;

  p=malloc(sizeof(int)*2);

  free(p);
  
  free(p);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Free3
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
