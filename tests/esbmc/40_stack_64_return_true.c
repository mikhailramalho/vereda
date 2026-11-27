int main() {
  int a; // 8 * 4 = 32
  int b; // 8 * 4 = 32 
  return 0; // Total 32 + 32 + 32 = 96
}
// Imported from ESBMC regression/esbmc/40_stack_64_return_true
// Needs: --stack-limit 96
// RUN: -m64 --stack-limit 96
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
