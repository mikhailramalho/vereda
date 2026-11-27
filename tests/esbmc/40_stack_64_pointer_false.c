int main() {
  int y = 5;  // 4 * 8 = 32
  int *yPtr;  // 8 * 8 = 64
  // Expected total here: 32+64= 96.
 
  yPtr = &y;
 
  int *xPtr; // 8 * 8 = 64
  // The total now is 96 + 64 = 160.
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_pointer_false
// Needs: --stack-limit 150
// RUN: -m64 --stack-limit 150 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
