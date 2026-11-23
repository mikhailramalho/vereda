// Widening on entry to check() must not reach the caller's parameter after the
// return.
void check(int c)
{
  __VERIFIER_assert(c); // i >= 0
}
void run(int i, int n)
{
  while (i < n)
  {
    check(i >= 0);
    i++;
  }
}
int main()
{
  int n = __VERIFIER_nondet_int();
  if (n > 100000)
    return 0;
  run(0, n);
}
// Imported from ESBMC regression/k-induction/interval_widening_callee_param
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// RUN: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
