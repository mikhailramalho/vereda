/* Only NaN violates the claim; one witness blocks every NaN payload. */
float __VERIFIER_nondet_float(void);
int main(void)
{
  float x = __VERIFIER_nondet_float();
  __VERIFIER_assert(x == x); // x is not NaN
  return 0;
}
// Imported from ESBMC regression/esbmc/github_10920_nan_class
// Needs: --all-witnesses --max-witnesses 3
// RUN: --all-witnesses --max-witnesses 3
// MODE: FUTURE
// EXPECT: ^\[Counterexamples - 1 witnesses\]$
// EXPECT: enumeration stopped: UNSAT after 1 witnesses
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
