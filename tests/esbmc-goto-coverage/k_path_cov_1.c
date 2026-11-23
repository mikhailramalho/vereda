// Two independent if statements — exercises k=2 prefix.
// Branch coverage needs 2 tests; k=2 path coverage exposes 4 paths
// (TT, TF, FT, FF) — all feasible.
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
// Imported from ESBMC regression/goto-coverage/k_path_cov_1
// Needs: --k-path-coverage=2
// RUN: --k-path-coverage=2 --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^k-Path Witnesses : 6$
// EXPECT: ^Spanning Set : 4$
// EXPECT: ^Reached : 4$
// EXPECT: ^k-Path Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
