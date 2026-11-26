/* Soundness guard for the branchy-body extension: both paths of an
 * if/else in the loop body must strictly decrease the measure, not just
 * one. Here the then-arm decrements i (m = 255 - i decreases by 1) but
 * the else-arm increments it (m increases by 1), so the loop does not
 * provably terminate (it can stay above the guard forever by picking the
 * else-arm). recognize_loop accepts the shape and yields two paths; the
 * decrease obligation on the else-path is satisfiable (m'_else = m + 1
 * >= m holds), so the prover must NOT certify the loop and control must
 * fall through to the existing machinery. Going up then down repeats a
 * state, which the recurrent-state check reports.
 *
 * Expected verdict: VERIFICATION FAILED. */

extern int __VERIFIER_nondet_int(void);

int main()
{
  int i = __VERIFIER_nondet_int();
  while (i < 255)
  {
    if (__VERIFIER_nondet_int() != 0)
      i = i + 1; /* progress toward guard becoming false */
    else
      i = i - 1; /* moves AWAY from the guard becoming false */
  }
  return 0;
}
// Imported from ESBMC regression/termination/ranking_branchy_one_path_grows
// Needs: --force-realloc-success --termination --max-inductive-step 3
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --no-align-check --k-step 2 --no-vla-size-check -m32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// EXPECT: ^Recurrent state shows a non-terminating execution.*$
// EXPECT: ^VERIFICATION FAILED$
// MODE: FUTURE
// Vereda: verdict none
