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
// Imported from ESBMC regression/esbmc/40_stack_64_function_call_true
// Needs: --stack-limit 16
// RUN: --64 --stack-limit 16
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
