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
// Imported from ESBMC regression/goto2lean/midpoint
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^def mid \(lo : BitVec 32\) \(hi : BitVec 32\) \(post : BitVec 32 → Prop\) : Prop :=$
// EXPECT: ^  post \(BitVec\.smtUDiv \(lo \+ hi\) \(2#32\)\)$
// EXPECT: ^  ∀ lo : BitVec 32,$
// EXPECT: ^  \(lo ≤ hi\) →$
// EXPECT: ^  \(\(lo ≤ m\) ∧ \(m ≤ hi\)\) ∧ \($
// EXPECT: ^  all_goals bv_decide$
// Vereda: verdict none
