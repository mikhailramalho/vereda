/* Unsigned counter entering at 5 with a SYMBOLIC addend. The symbolic
 * multiplier confines the pass to the two-disjunct bound, which is only
 * establishable for an entry value of 0 (or 1 under `<=`): at entry 5 with
 * n <= 5 the loop never runs, i stays 5, and neither `i < n` nor `i == n`
 * holds. Must decline. The literal-addend regime would admit this entry
 * value via its third disjunct, so this pins the boundary between the two. */
#include <assert.h>
unsigned int __VERIFIER_nondet_uint();
int main(void)
{
  unsigned int n = __VERIFIER_nondet_uint(), a = __VERIFIER_nondet_uint();
  __VERIFIER_assume(n <= 8);
  __VERIFIER_assume(a <= 3);
  unsigned int i = 5, s = 0;
  while (i < n)
  {
    s = s + a;
    i = i + 1;
  }
  assert(s <= 9);
  return 0;
}
// Imported from ESBMC regression/esbmc/synth_loop_invariant_entryfive
// Needs: --synthesise-loop-invariants
// RUN: --synthesise-loop-invariants --unwind 10
// MODE: FUTURE
// EXPECT: \A(?![\s\S]*Synthesised loop invariants)[\s\S]*\Z
// EXPECT: ^VERIFICATION SUCCESSFUL$
// Vereda: verdict none
