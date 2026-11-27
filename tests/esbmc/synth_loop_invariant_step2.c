/* Non-unit step: the closed form would need a division by the step, which is a
 * solver hazard, so the recogniser only accepts i = i + 1 and declines here.
 * Declining must leave the verdict alone. */
#include <assert.h>

int main(void)
{
  unsigned int i = 0;
  unsigned int s = 0;

  while (i < 10)
  {
    s = s + 1;
    i = i + 2;
  }

  assert(s == 5);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_step2
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
