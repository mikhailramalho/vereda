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
// RUN: --multi-property --ub-pointer-check=false --ub-div-by-zero-check=false --vla-size-check=false
// EXPECT: ^VERIFICATION FAILED$
