// Loop body with one branch; tests that prefix extends across
// unrolled iterations (Huang, Meyer & Weber, ICTSS 2025).
int main()
{
  int x;
  for (int i = 0; i < 3; i++)
  {
    if (x > 0)
      x = x - 1;
    else
      x = x + 1;
  }
  return x;
}
// Imported from ESBMC regression/goto-coverage/k_path_cov_2
// Needs: --k-path-coverage=2
// RUN: --k-path-coverage=2 --unwind 4 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^k-Path Witnesses : 6$
// EXPECT: ^Spanning Set : 4$
// EXPECT: ^Reached : 2$
// EXPECT: ^k-Path Coverage: 50%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
