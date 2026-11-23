#include <assert.h>
unsigned __VERIFIER_nondet_uint(void);
/* LLM-style candidate: midpoint of [lo, hi] */
unsigned mid(unsigned lo, unsigned hi) {
  return (lo + hi) / 2;
}
int main(void) {
  unsigned lo = __VERIFIER_nondet_uint(), hi = __VERIFIER_nondet_uint();
  __VERIFIER_assume(lo <= hi);
  unsigned m = mid(lo, hi);
  assert(lo <= m && m <= hi);
  return 0;
}
// Imported from ESBMC regression/goto2lean/overflow_check
// Needs: --goto2lean
// RUN: --goto2lean --unsigned-overflow-check
// MODE: FUTURE
// EXPECT: ^  ¬\(\(BitVec\.uaddOverflow lo hi\) = true\) ∧ \($
// EXPECT: ^  ¬\(\(\(2#32\) == 0#32\) = true\) ∧ \($
// EXPECT: ^  post \(BitVec\.smtUDiv \(lo \+ hi\) \(2#32\)\)\)\)$
