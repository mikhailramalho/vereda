int main() {
  int a; // 8 * 4 = 32
  int b; // 8 * 4 = 32 
  return 0; // Total 32 + 32 + 32 = 96
}
// Imported from ESBMC regression/esbmc/40_stack_64_return_false
// Needs: --stack-limit 95 --no-return-value-opt
// RUN: --64 --stack-limit 95 --no-return-value-opt
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
