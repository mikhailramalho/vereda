int main()
{
  
  float a, b, c;

  a = __VERIFIER_nondet_float();
  b = __VERIFIER_nondet_float();

  if (__VERIFIER_nondet_bool())
    c = a + b;
  else
    c = a - b;

  assert(c==a+b || c==a-b);
}
// Imported from ESBMC regression/cbmc/01_cbmc_Fixedbv7
// Needs: --fixedbv
// RUN: --fixedbv -Wno-error=implicit-function-declaration
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
