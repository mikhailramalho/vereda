int main() {
  int a; // 8
  while(a > 0) {
    int b; //8
    a--;
  }
  // Total: 16
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_while_true
// Needs: --k-induction --stack-limit 16
// RUN: --64 --k-induction --stack-limit 16
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
