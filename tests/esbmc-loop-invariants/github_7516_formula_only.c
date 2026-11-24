/* #7516: a formula-only run decides no base case, so the forward condition
   and inductive step still emit their formulas. */
int main()
{
  int i = 0, s = 0;
  __ESBMC_loop_invariant(i >= 0);
  while (i < 3)
  {
    s++;
    i++;
  }
  __VERIFIER_assert(s == 3); // sum
  return 0;
}
// Imported from ESBMC regression/loop-invariants/github_7516_formula_only
// Needs: --k-induction --smtlib --output phase.smt2
// RUN: --k-induction --max-k-step 2 --smtlib --smt-formula-only --output phase.smt2
// MODE: FUTURE
// EXPECT: ^Checking forward condition, k = 1$
// EXPECT: ^Checking inductive step, k = 2$
// Vereda: verdict none
