int main()
{
  int i, s = 0;
  for (i = 0; i < 3; i++)
    s++;
  __VERIFIER_assert(s == 3); // s_eq_3
}
// Imported from ESBMC regression/k-induction/github_6093_pass
// Needs: --k-induction --base-k-step 2
// RUN: --k-induction --base-k-step 2 --max-k-step 10
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
