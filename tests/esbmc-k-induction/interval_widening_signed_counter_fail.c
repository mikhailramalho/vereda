// The bound must not hide a real violation.
int a[100];
int main()
{
  a[3] = 1;
  for (int j = 0; j < 100; j++)
    __VERIFIER_assert(a[j] == 0); // zero
}
// Imported from ESBMC regression/k-induction/interval_widening_signed_counter_fail
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// RUN: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
