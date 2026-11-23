int main() {
  signed int i;
  
  i++;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Overflow_Addition1
// RUN: --overflow-check
// EXPECT: ^VERIFICATION FAILED$
