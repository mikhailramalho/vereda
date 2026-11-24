/* A user invariant on the outer loop protects the inner one too; see
 * enclosed_by_user_invariant for why. synth_loop_invariant_nestedinv_off is the
 * same program without the outer marker, where the inner loop is synthesised. */
#include <assert.h>

int main(void)
{
  unsigned int o = 0;
  unsigned int total = 0;

  __ESBMC_loop_invariant(o <= 2 && total == 4 * o);
  while (o < 2)
  {
    unsigned int i = 0;
    unsigned int s = 0;
    while (i < 4)
    {
      s = s + 1;
      i = i + 1;
    }
    total = total + s;
    o = o + 1;
  }

  assert(total == 8);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_nestedinv
// Needs: __ESBMC_loop_invariant
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 6
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^WARNING: --synthesise-loop-invariants added no invariant: no loop matched the affine counter/accumulator shape, or every match was declined; the run proceeds unchanged$
// EXPECT: ^  PASSED       \[main\.assertion\.\d\]  line 12  loop invariant base case$
// EXPECT: ^VERIFICATION SUCCESSFUL$
