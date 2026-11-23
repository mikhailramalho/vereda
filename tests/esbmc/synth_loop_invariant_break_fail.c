/* synth_loop_invariant_break with the assertion the loop violates. Declining a
 * loop must cost only a missed invariant: had the recogniser accepted this one
 * and cut it, the guard's iteration count would have licensed s == 6 and the
 * bug would read as a proof. */
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

  assert(s == 6);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_break_fail
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 8
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^WARNING: --synthesise-loop-invariants added no invariant: no loop matched the affine counter/accumulator shape, or every match was declined; the run proceeds unchanged$
// EXPECT: ^VERIFICATION FAILED$
// Vereda: verdict none
