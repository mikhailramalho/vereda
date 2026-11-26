int main() {
  int a; // 8
  while(a > 0) {
    int b; //8
    a--;
  }
  // Total: 16*8 = 128
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_while_false
// Needs: --k-induction --stack-limit 15
// RUN: -m64 --k-induction --stack-limit 15
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
