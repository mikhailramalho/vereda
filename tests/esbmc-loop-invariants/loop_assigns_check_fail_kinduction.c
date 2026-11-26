/* k-induction havocs the loop's variables ahead of its head; the assigns
 * clause must still be found and its compliance checked (#11081). The
 * declarations between the clause and the loop leave room for only a few of
 * those havocs inside the search window. */
int main()
{
  unsigned i = 0, n = 10, x = 0;
  __ESBMC_loop_invariant(i <= n);
  __ESBMC_loop_assigns(i);
  unsigned p0 = 1, p1 = 2, p2 = 3, p3 = 4;
  while (i < n)
  {
    i++;
    x++; /* not in __ESBMC_loop_assigns */
  }
  return 0;
}
// Imported from ESBMC regression/loop-invariants/loop_assigns_check_fail_kinduction
// Needs: __ESBMC_loop_assigns, __ESBMC_loop_invariant
// Needs: --loop-invariant-check --loop-frame-rule --k-induction
// RUN: --loop-invariant-check --loop-frame-rule --k-induction --max-k-step 5
// MODE: FUTURE
// EXPECT: ^Processing 1 loop invariant$
// EXPECT: ^.*x not in assigns clause.*$
// EXPECT: ^VERIFICATION FAILED$
