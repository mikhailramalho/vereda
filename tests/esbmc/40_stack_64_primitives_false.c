int main() {
  int stack_size; // 4 * 8 = 32
  char ptr; // 1 * 8 = 8
  // Expected total here: 8+32= 40.
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_primitives_false
// Needs: --stack-limit 32
// RUN: -m64 --stack-limit 32
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
