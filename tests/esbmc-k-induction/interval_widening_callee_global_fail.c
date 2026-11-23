int j;
void check(int c)
{
  __VERIFIER_assert(c); // j >= 1
}
int main()
{
  int n = __VERIFIER_nondet_int();
  if (n > 100000)
    return 0;
  for (j = 0; j < n; j++)
    check(j >= 1);
}
// Imported from ESBMC regression/k-induction/interval_widening_callee_global_fail
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// RUN: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
