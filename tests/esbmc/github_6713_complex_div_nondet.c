/* A nondeterministic dividend keeps the division out of the simplifier, so
   the quotient's VCC actually reaches the solver rather than passing
   vacuously (#6713). */
float __VERIFIER_nondet_float(void);

int main(void)
{
  float x = __VERIFIER_nondet_float();
  __VERIFIER_assume(x > 2.0f && x < 100.0f);

  _Complex float a = x, b = 2.0f;
  a /= b;

  __VERIFIER_assert(__real__ a > 1.0f); // quotient stays above one
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6713_complex_div_nondet
// RUN: --incremental-bmc
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
