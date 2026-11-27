int main() {
  int i;
  
  for(i=0; i<10; i++) {
    const int a=i;
  }
   
  assert(0);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Unwinding_Locality1
// EXPECT: ^VERIFICATION FAILED$
