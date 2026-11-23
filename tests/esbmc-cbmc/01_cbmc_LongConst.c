int main() {
  long int x=0x7FFFFFFFL;
  assert(x>0);
}
// Imported from ESBMC regression/cbmc/01_cbmc_LongConst
// Needs: --16
// RUN: --16  -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
