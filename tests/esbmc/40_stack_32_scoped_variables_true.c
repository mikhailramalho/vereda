int main()
{ 
  int a, b; // 4*8 + 4*8 = 64
  { 
    int a, b; // 4*8 + 4*8 = 64
  }
}
// Imported from ESBMC regression/esbmc/40_stack_32_scoped_variables_true
// Needs: --stack-limit 128 REQUIRES arch32
// RUN: --32 --stack-limit 128
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
