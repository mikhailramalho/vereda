int main() {
  char arr[10]; // alloca 10 * i8 = 80
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_array_false
// Needs: --stack-limit 79
// RUN: -m64 --stack-limit 79
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
