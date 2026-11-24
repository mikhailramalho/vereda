// GitHub #6154: an if/else condition that folds to a compile-time constant
// because it depends only on a call-site literal, not the quantifier's bound
// variable.  pick(v, 1) takes the then-branch unconditionally; pick(v, 0)
// takes the else-branch unconditionally.
int pick(int x, int mode)
{
  int r;
  if (mode == 1)
    r = x;
  else
    r = x + 1;
  return r;
}

int main()
{
  int v;
  __VERIFIER_assert(
    __ESBMC_forall(&v, pick(v, 1) == v)); // fold-true branch summarized
  __VERIFIER_assert(
    __ESBMC_forall(&v, pick(v, 0) == v + 1)); // fold-false branch summarized
  return 0;
}
// Imported from ESBMC regression/z3/github_6154_ifelse_fold
// RUN: --solver z3
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: unmet EXPECT: ^VERIFICATION SUCCESSFUL$
// MODE: FUTURE
// Vereda: verdict FAILED
