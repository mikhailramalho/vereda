// Function call: instrumentation visits each function independently
// (per-function k-path scope, #4325). Each function contributes its own
// branch witnesses; goal count adds up across callers and callees.
int helper(int z)
{
  if (z > 0)
    return z + 1;
  return -z;
}

int main()
{
  int a;
  if (a > 0)
    return helper(a);
  return helper(-a);
}
// Imported from ESBMC regression/goto-coverage/k_path_cov_3
// Needs: --k-path-coverage=2
// RUN: --k-path-coverage=2 --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^k-Path Witnesses : 4$
// EXPECT: ^Reached : 4$
// EXPECT: ^k-Path Coverage: 100%$
// EXPECT: ^COVERAGE ANALYSIS COMPLETE$
// Vereda: verdict none
