int main()
{
  int x;
  int r = 0;
  if (x < 0)
    r = 1;
  if (x > 10)
    r = 2;
  return r;
}
// Imported from ESBMC regression/goto-coverage/k_path_cov_16_range_contradiction
// Needs: --k-path-coverage=2
// RUN: --k-path-coverage=2 --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^k-Path Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
