/* Entry value 2 is outside the two-disjunct range, but with a literal addend
 * the three-disjunct bound is affordable and the loop is summarised. The
 * i >= i0 conjunct is still emitted here because the counter is unsigned. */
#include <assert.h>

int main(void)
{
  unsigned int n;
  __VERIFIER_assume(n <= 6);

  unsigned int i = 2;
  unsigned int s = 0;

  while (i < n)
  {
    s = s + 1;
    i = i + 1;
  }

  assert(s <= 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_entrytwo
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 6
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
