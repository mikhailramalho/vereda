/* Signed counter with a SYMBOLIC addend. That needs the two-disjunct bound,
 * which is only establishable for an unsigned counter starting at 0 or 1, so
 * the relaxation must not apply here. Declines. */
#include <assert.h>
int __VERIFIER_nondet_int();
int main(void)
{
  int n = __VERIFIER_nondet_int(), a = __VERIFIER_nondet_int();
  __VERIFIER_assume(n >= 1 && n <= 6);
  __VERIFIER_assume(a >= 0 && a <= 3);
  int i = 1, sn = 0;
  while (i <= n) { sn = sn + a; i++; }
  assert(sn == n * a || sn == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_signed_symbolic
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 10
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
