/* Signed counter with an assertion inside the body. A signed counter cannot
 * carry the i >= i0 conjunct, so the havoc could pick i < i0 where i - i0 is
 * negative and the closed form describes an unreachable state -- which only
 * shows up when something reads the accumulator mid-loop. Must decline, and
 * must leave the verdict alone. */
#include <assert.h>
int __VERIFIER_nondet_int();
int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n >= 1 && n <= 10);
  int i = 1, sn = 0;
  while (i <= n)
  {
    assert(sn <= 2 * n);
    sn = sn + 2;
    i++;
  }
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_signed_inloop
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 14
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
