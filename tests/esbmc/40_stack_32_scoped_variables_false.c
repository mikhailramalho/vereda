int main()
{ 
  int a, b; // 4*8 + 4*8 = 64
  { 
    int a, b; // 4*8 + 4*8 = 64
  }
}
// Imported from ESBMC regression/esbmc/40_stack_32_scoped_variables_false
// Needs: --stack-limit 127
// RUN: --32 --stack-limit 127
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
