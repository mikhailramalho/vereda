// Widening on entry to check() dropped the bound j < n gives, so j + 1 could
// overflow and j came back unbounded.
void check(int c)
{
  __VERIFIER_assert(c); // j >= 0
}
int main()
{
  int n = __VERIFIER_nondet_int();
  if (n > 100000)
    return 0;
  for (int j = 0; j < n; j++)
    check(j >= 0);
}
// Imported from ESBMC regression/k-induction/interval_widening_callee
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// RUN: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
