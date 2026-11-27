// GitHub #6154: an if/else function (no loop) inside __ESBMC_exists.  sgn is
// summarized into (x > 0 ? 1 : 0); the existential holds for any x > 0.
int sgn(int x)
{
  int r;
  if (x > 0)
    r = 1;
  else
    r = 0;
  return r;
}

int main()
{
  int v;
  __VERIFIER_assert(__ESBMC_exists(&v, sgn(v) == 1)); // sgn is 1 for some v
  return 0;
}
// Imported from ESBMC regression/z3/github_6154_ifelse_exists
// Needs: __ESBMC_exists
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
