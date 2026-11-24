/* Combined mode composes with --termination where the standalone schema cannot.
 * goto_loop_invariant_combined splices its verification branch before the loop
 * head with destructive_insert, so the head is still the guard GOTO when
 * goto_termination's havoc reaches it; goto_loop_invariant uses insert_swap and
 * leaves an ASSERT there, which aborts. The bound is symbolic and the exit test
 * is `!=`, so the ranking check does not settle the run before the marker
 * transform gets to it. */
unsigned int __VERIFIER_nondet_uint(void);

int main(void)
{
  unsigned int i = 0;
  unsigned int n = __VERIFIER_nondet_uint();
  unsigned int s = 0;

  __ESBMC_loop_invariant(s == i);
  while (i != n)
  {
    s = s + 1;
    i = i + 1;
  }

  return 0;
}
// Imported from ESBMC regression/esbmc/loop_invariant_combined_termination
// Needs: __ESBMC_loop_invariant
// Needs: --loop-invariant --termination
// RUN: --loop-invariant --termination
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*cannot be combined with --termination)[\s\S]*\Z
// EXPECT: ^VERIFICATION UNKNOWN$
