int main(void)
{
  int x = 0, i = 0;
  if (1)
  {
    x = 0;
  }
  while (i++ < 2)
    ++x;
  do
    ++x;
  while (i++ < 4);
  if (x == 4)
    ++x;
  else
    x += 2;
  if (0)
    x += 10;
  else
    ++x;
  __VERIFIER_assert(x == 6); // control bodies
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/injected_control_bodies
// Needs: --sv-comp --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// RUN: --sv-comp -m32 --ub-div-by-zero-check=false --k-step 2 --unlimited-k-steps --vla-size-check=false --ub-pointer-check=false --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict none
