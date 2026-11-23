// Widening only at loop back edges left recursion unbounded: the analysis
// recursed until it overflowed the stack.
int sum(int n)
{
  if (n == 0)
    return 0;
  return n + sum(n - 1);
}
int main()
{
  __VERIFIER_assert(sum(5) == 15); // sum
}
// Imported from ESBMC regression/k-induction/interval_widening_recursion
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// RUN: --k-induction --interval-analysis --interval-analysis-arithmetic --interval-analysis-extrapolate --interval-analysis-extrapolate-under-approximate
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
