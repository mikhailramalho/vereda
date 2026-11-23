unsigned mid(unsigned lo, unsigned hi) {
  __VERIFIER_assume(lo <= hi);
  unsigned m = (lo + hi) / 2;
  __VERIFIER_assert(lo <= m); // lo<=m
  return m;
}
int main(void) { return 0; }
// Imported from ESBMC regression/goto2lean/entry_with_parameters
// Needs: --goto2lean
// RUN: --goto2lean --function mid
// MODE: FUTURE
// EXPECT: ^theorem mid_safe : ∀ \(lo : BitVec 32\) \(hi : BitVec 32\), Prog\.mid lo hi \(fun _ => True\) := by$
// Vereda: verdict none
