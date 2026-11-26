// The exit test reads x and y after the body wrote them, but nothing writes
// them after it, so x >= 0 && y >= 0 holds at every head after the first, and
// at the first because it holds before the loop.
int main()
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  if (!(x == y && y >= 0))
    return 0;
  while (x != 0)
  {
    x--;
    y--;
    if (x < 0 || y < 0)
      break;
  }
}
// Imported from ESBMC regression/k-induction/entry_cond_exit_test_invariant
// Needs: --k-induction --interval-analysis --interval-analysis-arithmetic
// RUN: --k-induction --max-k-step 10 --overflow-check --no-assertions --interval-analysis --interval-analysis-arithmetic
// MODE: FUTURE
// EXPECT: ^Solution found by the inductive step \(k = 2\)$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
