/* Counter-only loop: the synthesised bound (i < n || i == n || i == 0) is
 * enough on its own; no accumulator conjunct is needed. */
#include <assert.h>

int main(void)
{
  unsigned int n;
  unsigned int i = 0;

  while (i < n)
    i++;

  assert(i == n || n == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_counter
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --no-incremental-bmc
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
