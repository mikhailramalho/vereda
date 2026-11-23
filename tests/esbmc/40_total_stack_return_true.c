int main() {
  int a; // 8 * 4 = 32
  int b; // 8 * 4 = 32 
  return 0; // Total 32 + 32 + 32 = 96
}
// Imported from ESBMC regression/esbmc/40_total_stack_return_true
// Needs: --total-stack-limit 96 --no-return-value-opt
// RUN: --64 --total-stack-limit 96 --no-return-value-opt
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
