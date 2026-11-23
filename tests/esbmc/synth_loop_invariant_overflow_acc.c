/* Unsigned counter, SIGNED accumulator, overflow checking on. The closed form
 * is built at the accumulator's type, not the counter's, so `s0 + (i - i0) * e`
 * here is signed arithmetic and goto_check draws overflow claims on operations
 * the user never wrote. The counter alone being unsigned is not enough: the
 * decline has to look at every accumulator's type too. Must decline, and the
 * user's own verdict must be unaffected. */
#include <assert.h>
unsigned int __VERIFIER_nondet_uint();
int main(void)
{
  unsigned int n = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 8);
  unsigned int i = 0;
  int sn = 0;
  while (i < n)
  {
    sn = sn + 2;
    i = i + 1;
  }
  assert(sn == 2 * (int)n);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_overflow_acc
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --overflow-check --unwind 12
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
