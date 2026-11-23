int __VERIFIER_nondet_int();

int main()
{
  int x = __VERIFIER_nondet_int();
  __VERIFIER_assume(x >= 0 && x < 4);
  int i;
  for (i = 0; i < 4; i++)
  {
    __VERIFIER_assert(x + i >= 0); // no_underflow
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/github_6070_pass
// Needs: --k-induction --cex-output out.cex
// RUN: --k-induction --multi-property --cex-output out.cex --unwind 5
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
