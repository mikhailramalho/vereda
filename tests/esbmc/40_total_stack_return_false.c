int main() {
  int a; // 8 * 4 = 32
  int b; // 8 * 4 = 32 
  return 0; // Total 32 + 32 + 32 = 96
}
// Imported from ESBMC regression/esbmc/40_total_stack_return_false
// Needs: --total-stack-limit 95
// RUN: -m64 --total-stack-limit 95
// MODE: FUTURE
// EXPECT: Total stack limit property was violated
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
