void foo()
{
  char a;
  char b;
  // Total: 16
}
int main() {
  foo(); // total: 16
  return 0;
}
// Imported from ESBMC regression/esbmc/40_stack_64_function_call_false
// Needs: --stack-limit 15
// RUN: -m64 --stack-limit 15
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
