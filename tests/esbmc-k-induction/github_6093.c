int main()
{
  int idx, max, res;

  __VERIFIER_assume(0 <= max);
  res = 0;
  idx = 0;

  while (idx < max)
  {
    ++idx;
    ++res;
  }
  __VERIFIER_assert(idx == max); // A.1
}
// Imported from ESBMC regression/k-induction/github_6093
// Needs: --loop-invariant --base-k-step 0
// RUN: --loop-invariant --base-k-step 0 --max-k-step 2
// MODE: FUTURE
// EXPECT: ^ERROR: Please specify --base-k-step >= 1
// EXPECT: ^ESBMC version
// Vereda: verdict none
