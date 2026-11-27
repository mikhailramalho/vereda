int main() {
  signed int i;
  
  i++;
}
// Imported from ESBMC regression/cbmc/01_cbmc_Overflow_Addition1
// RUN: --ub-signed-overflow-check --ub-shift-check --ub-float-cast-overflow-check
// EXPECT: ^VERIFICATION FAILED$
