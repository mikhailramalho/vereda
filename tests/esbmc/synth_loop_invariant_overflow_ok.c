/* Counter and accumulator both unsigned, overflow checking on. Unsigned wrap is
 * well defined, so no claim goto_check instruments on the synthesised
 * arithmetic can invent a failure -- the decline must NOT fire here. Pins the
 * admitting side of the overflow rule: a decline that ignored the accumulator
 * types would still pass, but one that declined on any accumulator under
 * --overflow-check would lose this loop's invariant. */
#include <assert.h>
unsigned int __VERIFIER_nondet_uint();
int main(void)
{
  unsigned int n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 8);
  unsigned int i = 0, s = 0;
  while (i < n)
  {
    s = s + 2;
    i = i + 1;
  }
  assert(s == 2 * n);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_overflow_ok
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --overflow-check --unwind 12
// MODE: FUTURE
// EXPECT: ^Synthesised loop invariants for 1 loop$
// EXPECT: ^VERIFICATION SUCCESSFUL$
