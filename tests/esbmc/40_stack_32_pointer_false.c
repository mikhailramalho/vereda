int main() {
  int y = 5;  // 4 * 8 = 32
  int *yPtr;  // 32
  // Expected total here: 32+32= 64.

  yPtr = &y;

  int *xPtr; // 32
  // The total now is 64 + 32 = 96.
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_32_pointer_false
// Needs: --stack-limit 95
// RUN: -m32 --stack-limit 95 --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
