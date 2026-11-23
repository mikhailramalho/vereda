int main()
{
  int x;
  int r = 0;
  if (x == 1)
    x = 2;
  if (x == 2)
    r = 2;
  return r;
}
// Imported from ESBMC regression/goto-coverage/k_path_cov_17_eq_mutated
// Needs: --k-path-coverage=2
// RUN: --k-path-coverage=2 --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^k-Path Coverage: 66.66666666666667%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
