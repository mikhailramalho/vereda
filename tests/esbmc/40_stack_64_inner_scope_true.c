int main() {
  int a; // 4 * 8 = 32
  {
    int a; // 4 * 8 = 32
    // Total: 64
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_inner_scope_true
// Needs: --stack-limit 64
// RUN: --64 --stack-limit 64
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
