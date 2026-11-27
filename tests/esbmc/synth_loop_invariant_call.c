/* A call in the body can write the counter or an accumulator out of sight, so
 * the recogniser declines rather than summarise a body it cannot see. */
#include <assert.h>

static unsigned int step(unsigned int x)
{
  return x + 1;
}

int main(void)
{
  unsigned int i = 0;
  unsigned int s = 0;

  while (i < 4)
  {
    s = s + step(0);
    i = i + 1;
  }

  assert(s == 4);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_call
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^WARNING: --synthesise-loop-invariants added no invariant: no loop matched the affine counter/accumulator shape, or every match was declined; the run proceeds unchanged$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
