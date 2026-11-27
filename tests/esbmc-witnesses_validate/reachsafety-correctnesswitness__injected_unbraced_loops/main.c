int main(void)
{
  int x = 0;
  unsigned int i, j;
  for (i = 0; i < 2; ++i)
    for (j = 0; j < 2; ++j)
      ++x;
  __VERIFIER_assert(x == 4); // iteration count
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/injected_unbraced_loops
// Needs: --sv-comp --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// RUN: --sv-comp -m32 --ub-div-by-zero-check=false --no-align-check --k-step 2 --unlimited-k-steps --vla-size-check=false --ub-pointer-check=false --interval-analysis --no-bounds-check --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
