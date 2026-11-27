// GitHub #6154: discriminating failing case for the refused float `++'.
// Falling back to normal unwinding must still evaluate the body, so the false
// claim f(k) == k + 99 is detected rather than passing vacuously.
int f(int n)
{
  double d = 0.0;
  for (int i = 0; i < 3; i++)
    d++;
  return n + (int)d;
}

int main()
{
  int k;
  __VERIFIER_assert(__ESBMC_forall(&k, f(k) == k + 99)); // should fail
  return 0;
}
// Imported from ESBMC regression/z3/github_6154_reject_float_inc_fail
// Needs: __ESBMC_forall
// RUN: --solver z3 --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
