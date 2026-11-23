int main() {
  char arr[10]; // alloca 10 * i8 = 80
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_array_false
// Needs: --stack-limit 79
// RUN: --64 --stack-limit 79
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
