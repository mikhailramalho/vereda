/* Same program as synth_loop_invariant_sum, run with --interval-analysis.
 * That pass rewrites the loop head into the simplified `IF i > n GOTO exit`,
 * which has no not2t to strip, and moves the back-edge target ahead of the
 * guard; the recogniser must still match, and the marker must still land where
 * goto_loop_invariant's extractor searches. See that test for why the widths
 * are small. */
#include <stdint.h>
#include <assert.h>

int main(void)
{
  uint32_t i = 1;
  uint32_t sn = 0;
  uint8_t n;
  uint8_t a;

  __VERIFIER_assume(n >= 1 && n <= 15);

  while (i <= n)
  {
    sn = sn + a;
    i++;
  }

  assert(sn == (uint32_t)n * a);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_interval
// Needs: --synthesise-loop-invariants --interval-analysis
// RUN: --synthesise-loop-invariants --interval-analysis
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
