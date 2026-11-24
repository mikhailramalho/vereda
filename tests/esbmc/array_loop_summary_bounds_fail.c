// The summary keeps one iteration at a nondet index, so its bounds check
// still finds the symbolic overrun.
int a[50];
int __VERIFIER_nondet_int();
int main()
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n <= 51);
  for (int i = 0; i < n; i++)
    a[i] = 1;
}
// Imported from ESBMC regression/esbmc/array_loop_summary_bounds_fail
// RUN: --summarize-array-loops
// EXPECT: ^VERIFICATION FAILED$
// Vereda: unmet EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict SUCCESSFUL
