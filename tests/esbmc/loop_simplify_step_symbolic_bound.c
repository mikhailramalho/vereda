/* Step recognition handles loops where the bound is a symbol whose
 * value is pinned by an __VERIFIER_assume to a singleton interval.
 * Interval analysis folds the bound into a constant_int AND rewrites
 * `!(i < 10)` into `i >= 10`, so the IF guard ends up un-negated.
 * parse_guard accepts both negated and un-negated exit shapes. */
int main()
{
  unsigned N;
  __VERIFIER_assume(N == 10);

  int i;
  for (i = 0; i < (int)N; i++)
    ;
  __VERIFIER_assert(i == 10); // symbolic bound folded by interval analysis
  return 0;
}
// Imported from ESBMC regression/esbmc/loop_simplify_step_symbolic_bound
// Needs: --interval-analysis
// RUN: --interval-analysis --unwind 1 --no-unwinding-assertions
// MODE: FUTURE
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
