/* The accumulator's addend is the counter, which the loop modifies. The
 * closed form s0 + (i - i0) * e is only valid for an e that is constant across
 * the loop, so the recogniser declines. */
#include <assert.h>

int main(void)
{
  unsigned int n;
  __VERIFIER_assume(n <= 4);

  unsigned int i = 0;
  unsigned int s = 0;

  while (i < n)
  {
    s = s + i;
    i = i + 1;
  }

  assert(s <= 6);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_addendvar
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 6
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
