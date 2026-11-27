/* Rebinding boundary for the copy-chain match: the caller's guard pins
 * the FIRST generation of x, and the rebinding mints a new one, so the
 * callee's re-check is NOT implied and must still fork — x can be
 * nonnegative again. A match keyed on the base name instead of the
 * generation would subsume the stale guard and prove this vacuously. */
int __VERIFIER_nondet_int(void);

static int check(int v)
{
  if (v < 0)
    return 1;
  return 2;
}

int main(void)
{
  int x = __VERIFIER_nondet_int();
  if (x >= 0)
    return 0;
  x = __VERIFIER_nondet_int();
  __VERIFIER_assert(check(x) == 1); // x may be nonnegative after rebinding
  return 0;
}
// Imported from ESBMC regression/esbmc/path_guard_subsumption_rebind_fail
// RUN: --no-incremental-bmc
// EXPECT: ^VERIFICATION FAILED$
