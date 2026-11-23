/* A `break` leaves the loop from inside the body, so the number of iterations
 * is not the guard's and the closed form would describe a state the loop never
 * reaches. assertion_only_region rejects the jump out of the body and the
 * recogniser declines; BMC still proves the property.
 * synth_loop_invariant_break_fail is the same loop with a false assertion. */
#include <assert.h>

int main(void)
{
  unsigned int i = 0;
  unsigned int s = 0;

  while (i < 6)
  {
    if (i == 3)
      break;
    s = s + 1;
    i = i + 1;
  }

  assert(s == 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_break
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 8
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^WARNING: --synthesise-loop-invariants added no invariant: no loop matched the affine counter/accumulator shape, or every match was declined; the run proceeds unchanged$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
