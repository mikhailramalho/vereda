
int main() {
  int *p;

  free(p);  
}
// Imported from ESBMC regression/cbmc/01_cbmc_Malloc7
// EXPECT: ^VERIFICATION FAILED$
