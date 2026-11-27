// GitHub #6154: a summarized callee inside __VERIFIER_assume must not over-prune.
// forall x . nonneg(x) >= 0 is unconditionally true (unlike |x| >= 0, which is
// false at INT_MIN), so the assume constrains nothing and the assert below is
// reachable.  A vacuous quantifier would silently make this SUCCESSFUL.
int nonneg(int a)
{
  if (a < 0)
    return 0;
  return a;
}

int main()
{
  int x;
  __VERIFIER_assume(__ESBMC_forall(&x, nonneg(x) >= 0));
  __VERIFIER_assert(0); // reachable: the assume is a tautology
  return 0;
}
// Imported from ESBMC regression/z3/github_6154_assume_summarized
// Needs: __ESBMC_forall
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
