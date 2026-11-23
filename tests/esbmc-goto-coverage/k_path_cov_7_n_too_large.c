// Regression for #4330 Copilot-3: N above the safe cap (30) must be
// rejected. Without the cap, `1 << pdepth` would overflow size_t for
// pdepth >= 64 and underflow goal-budget arithmetic for pdepth >= 62 —
// both unsound for a formal-verification tool.
int main()
{
  int a;
  return a > 0 ? 1 : 0;
}
// Imported from ESBMC regression/goto-coverage/k_path_cov_7_n_too_large
// Needs: --k-path-coverage=31
// RUN: --k-path-coverage=31 --unwind 2 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^ERROR: --k-path-coverage=N requires 1 <= N <= 30 \(got 31\)$
// Vereda: verdict none
