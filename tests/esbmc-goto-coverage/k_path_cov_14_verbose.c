int main()
{
  int a, b;
  if (a > 0)
    a = 1;
  else
    a = -1;

  if (b > 0)
    b = 1;
  else
    b = -1;

  return a + b;
}
// Imported from ESBMC regression/goto-coverage/k_path_cov_14_verbose
// Needs: --k-path-coverage=2
// RUN: --k-path-coverage=2 --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^Current k-Path Coverage: 100%$
// EXPECT: ^k-Path Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
