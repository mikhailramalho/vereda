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
  __VERIFIER_assert(x == 7); // control bodies
  return 0;
}
// Imported from ESBMC regression/witnesses_validate/reachsafety-correctnesswitness/injected_control_bodies_fail
// Needs: --sv-comp --state-hashing --add-symex-value-sets --interval-analysis --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// RUN: --sv-comp --32 --no-div-by-zero-check --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --unlimited-k-steps --no-vla-size-check --no-pointer-check --interval-analysis --no-bounds-check --error-label ERROR --k-induction --max-inductive-step 3 --validate-correctness-witness --witness witness.yml
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
