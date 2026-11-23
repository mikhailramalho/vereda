// Widening at a guard inside the body dropped j >= 0, so the inductive step
// started from j = -1 and read a[-1].
int a[100];
int main()
{
  for (int j = 0; j < 100; j++)
    __VERIFIER_assert(a[j] == 0); // zero
}
// Imported from ESBMC regression/k-induction/interval_widening_signed_counter
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// RUN: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
