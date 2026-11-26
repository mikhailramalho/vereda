/* Linear ranking-function termination, scalar counter.
 *
 * `while (n > 0) n = n - 1` terminates: the measure m = n (the guard's
 * difference n - 0) strictly decreases by 1 each iteration and the
 * guard implies m >= 1, so the loop runs at most n iterations.
 *
 * try_prove_termination_by_ranking recognizes the shape (relational
 * guard over a scalar, straight-line decrement body), builds the
 * widened measure, and discharges both obligations via the solver:
 *   bounded:  guard AND (m < 1) is UNSAT
 *   decrease: guard AND (m' >= m) is UNSAT
 * The forward condition would also eventually prove this, but only by
 * unwinding n times; the ranking proof is k-independent and immediate.
 *
 * Expected verdict: VERIFICATION SUCCESSFUL. */

extern int __VERIFIER_nondet_int(void);

int main()
{
  int n = __VERIFIER_nondet_int();
  while (n > 0)
    n = n - 1;
  return 0;
}
// Imported from ESBMC regression/termination/ranking_counter
// Needs: --force-realloc-success --no-assertions --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Ranking function shows all executions terminate$
// Vereda: verdict none
