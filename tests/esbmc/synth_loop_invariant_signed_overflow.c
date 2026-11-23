/* Signed counter under --overflow-check. goto_check instruments every
 * instruction guard, including the ones this pass emits, so the synthesised
 * (i - i0) * e would draw overflow claims on arithmetic the user never wrote.
 * Must decline while overflow checking is on. */
#include <assert.h>
int __VERIFIER_nondet_int();
int main(void)
{
  int n = __VERIFIER_nondet_int();
  __VERIFIER_assume(n >= 1 && n <= 10);
  int i = 1, sn = 0;
  while (i <= n) { sn = sn + 2; i++; }
  assert(sn == 2 * n || sn == 0);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_signed_overflow
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --overflow-check --unwind 14
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
