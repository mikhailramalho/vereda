int main() {
  int stack_size; // 4 * 8 = 32
  char ptr; // 1 * 8 = 8
  // Expected total here: 8+32= 40.
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_primitives_true
// Needs: --stack-limit 40
// RUN: -m64 --stack-limit 40 --no-incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
