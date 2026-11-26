int main() {
  char arr[10]; // alloca 10 * i8 = 80
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_array_true
// Needs: --stack-limit 80
// RUN: -m64 --stack-limit 80
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
