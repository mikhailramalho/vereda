int main() {
  int stack_size; // 4 * 8 = 32
  char ptr; // 1 * 8 = 8
  // Expected total here: 8+32= 40.
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_primitives_true
// Needs: --stack-limit 40
// RUN: --64 --stack-limit 40
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
