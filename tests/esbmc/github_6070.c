int main()
{
  int i;
  for (i = 0; i < 4; i++)
  {
    __VERIFIER_assert(i != 1); // fail_when_i_is_1
    __VERIFIER_assert(i != 3); // fail_when_i_is_3
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6070
// Needs: --k-induction --cex-output out.cex
// RUN: --k-induction --multi-property --cex-output out.cex --unwind 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
