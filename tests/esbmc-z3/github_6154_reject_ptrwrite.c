// GitHub #6154: a callee that writes through a pointer is NOT summarizable --
// the summarizer models only local scalars, so accepting this would drop the
// store.  It must be refused (and the quantifier then diagnosed) rather than
// silently turned into a pure expression.  __ESBMC_exists has no skolemization
// fallback, so refusal surfaces as the diagnostic below.
int store(int a, int *p)
{
  *p = a;
  return a;
}

int main()
{
  int q = 0;
  int v;
  __VERIFIER_assert(__ESBMC_exists(&v, store(v, &q) == 7)); // not summarizable
  return 0;
}
// Imported from ESBMC regression/z3/github_6154_reject_ptrwrite
// Needs: __ESBMC_exists
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: cannot model call to .* on a quantified variable inside __ESBMC_forall/__ESBMC_exists
