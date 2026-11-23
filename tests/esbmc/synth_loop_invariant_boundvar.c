/* The bound `n + m` mentions m, which the body writes. A bound that can move
 * under the loop is not a bound, so the recogniser declines. The body is
 * otherwise exactly the affine shape it accepts -- m's addend is zero, so the
 * loop still terminates -- which leaves the bound check as the only reason to
 * decline. */
#include <assert.h>

int main(void)
{
  unsigned int n, e;
  __VERIFIER_assume(n >= 1 && n <= 3);
  __VERIFIER_assume(e == 0);

  unsigned int m = 0;
  unsigned int i = 0;

  while (i < n + m)
  {
    m = m + e;
    i = i + 1;
  }

  assert(i <= 3);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_boundvar
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 7
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
