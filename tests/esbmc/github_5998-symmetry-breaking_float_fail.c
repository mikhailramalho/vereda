float __VERIFIER_nondet_float(void);

int main()
{
  float x = __VERIFIER_nondet_float();
  float max = 0.0f;
  if (x > max)
    max = x;
  __VERIFIER_assert(x <= max); // post-merge bound should fail for NaN x
  return 0;
}
// Imported from ESBMC regression/esbmc/github_5998-symmetry-breaking_float_fail
// Needs: --no-standard-checks
// RUN: --multi-property --no-standard-checks
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
