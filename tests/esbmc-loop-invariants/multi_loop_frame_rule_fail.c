/*
 * multi_loop_frame_rule_fail: Two sequential loops; the second loop
 * modifies k which is NOT in its assigns clause.
 *
 * Verifies that assigns compliance is correctly applied to the second loop
 * even after a first loop has already created and consumed its own
 * frame_enforcert snapshots.
 *
 * Expected: VERIFICATION FAILED (k not in assigns clause)
 */

int main()
{
  /* First loop: correct — assigns only i */
  int i = 0;
  __ESBMC_loop_invariant(i >= 0 && i <= 5);
  __ESBMC_loop_assigns(i);
  while (i < 5)
    i++;

  /* Second loop: violates assigns — k is modified but not listed */
  int j = 0, k = 0;
  __ESBMC_loop_invariant(j >= 0 && j <= 3);
  __ESBMC_loop_assigns(j);
  while (j < 3)
  {
    j++;
    k++; /* assigns violation */
  }

  return 0;
}
// Imported from ESBMC regression/loop-invariants/multi_loop_frame_rule_fail
// Needs: __ESBMC_loop_assigns, __ESBMC_loop_invariant
// Needs: --loop-invariant-check --loop-frame-rule --k-induction
// RUN: --loop-invariant-check --loop-frame-rule --k-induction
// MODE: FUTURE
// EXPECT: ^VERIFICATION FAILED$
