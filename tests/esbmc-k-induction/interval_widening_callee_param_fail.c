void check(int c)
{
  __VERIFIER_assert(c); // i >= 1
}
void run(int i, int n)
{
  while (i < n)
  {
    check(i >= 1);
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
// Imported from ESBMC regression/k-induction/interval_widening_callee_param_fail
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// RUN: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
