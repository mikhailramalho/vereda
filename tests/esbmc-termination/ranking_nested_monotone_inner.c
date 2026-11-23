// Pins the monotone-counter inner-loop post-condition refinement.
//
// The outer's continuation requires *x == *y && *x > 0*. The inner is
// a single-path body where every modified scalar has a unit-step
// assign (y--, x--), and the anchor y has step -1 with guard `y > 0`.
//
// Pure-havoc summary would make post-inner *x fresh nondet → no
// measure on *x decreases pre→post-outer → ranking fails. The
// refinement emits post-inner *y == 0 and *x == pre_x - pre_y,
// which combined with the outer's `*x == *y && *x > 0` precondition
// forces a contradiction after one iteration — ranking discharges.
extern int __VERIFIER_nondet_int(void);

int main(void)
{
  int x = __VERIFIER_nondet_int();
  int y = __VERIFIER_nondet_int();
  if (x <= 0 || y <= 0)
    return 0;
  while (x == y && x > 0)
  {
    while (y > 0)
    {
      x = x - 1;
      y = y - 1;
    }
  }
  return 0;
}
// Imported from ESBMC regression/termination/ranking_nested_monotone_inner
// Needs: --force-realloc-success --state-hashing --add-symex-value-sets --k-step 2 --no-assertions --termination --max-inductive-step 3 REQUIRES arch32
// RUN: --no-div-by-zero-check --force-malloc-success --force-realloc-success --state-hashing --add-symex-value-sets --no-align-check --k-step 2 --floatbv --no-vla-size-check --32 --no-pointer-check --no-bounds-check --no-assertions --termination --max-inductive-step 3
// MODE: FUTURE
// EXPECT: ^Ranking function shows all executions terminate$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
