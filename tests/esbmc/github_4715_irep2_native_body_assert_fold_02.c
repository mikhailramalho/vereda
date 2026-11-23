// The fall-through half of github_4715_irep2_native_body_assert_fold_01: a fold
// that fires and still leaves the other branch to convert re-enters
// generate_ifthenelse with the branches swapped, which the native arm delegates
// instead of reproducing. Pins that the delegation keeps the fold.
extern int nd(void);
int main(void)
{
  int c = nd();
  if (c == 1)
    __VERIFIER_assert(0); // x
  else
    return 7;
  if (c == 2)
    return 8;
  else
    __VERIFIER_assert(0); // y
  return 0;
}
// Imported from ESBMC regression/esbmc/github_4715_irep2_native_body_assert_fold_02
// Needs: --goto-functions-only
// RUN: --goto-functions-only
// MODE: FUTURE
// EXPECT: ASSERT !\(c == 1\) // x
// EXPECT: ASSERT c == 2 // y
// Vereda: verdict none
