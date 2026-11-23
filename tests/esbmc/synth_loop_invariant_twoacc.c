/* Two accumulators over one counter. Both closed forms must be emitted, and
 * in a deterministic order: the modified-variable set is hashed on interned
 * string order, which varies between runs, so the recogniser sorts before
 * emitting. */
#include <stdint.h>
#include <assert.h>

int main(void)
{
  uint32_t n;
  uint64_t a, b;
  __VERIFIER_assume(n >= 1 && n <= 3);

  uint64_t i = 1;
  uint64_t s = 0;
  uint64_t t = 0;

  while (i <= n)
  {
    s = s + a;
    t = t + b;
    i++;
  }

  assert(s == (uint64_t)n * a);
  assert(t == (uint64_t)n * b);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_twoacc
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
