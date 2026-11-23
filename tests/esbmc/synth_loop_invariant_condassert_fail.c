/* The counterpart of synth_loop_invariant_condassert: the same diamond, with a
 * claim the closed form does not imply. Recognising the region must not make
 * the abstraction prove it -- the cut loop reports the claim unknown rather
 * than passed, so a body whose assert is a branch cannot be a false proof. */
#include <assert.h>
#include <stdint.h>

int main(void)
{
  uint32_t n;
  uint64_t a;
  uint64_t i = 1, sn = 0;
  __VERIFIER_assume(n >= 1 && n <= 10);
  __VERIFIER_assume(a <= 10);
  while (i <= n)
  {
    if (i > 1)
      assert(sn > a);
    sn = sn + a;
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_condassert_fail
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 12
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: loop invariant too weak to prove this claim
// EXPECT: ^VERIFICATION UNKNOWN$
// Vereda: verdict none
