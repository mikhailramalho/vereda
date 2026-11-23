unsigned __VERIFIER_nondet_uint(void);
unsigned id(unsigned v) { return v; }
int main(void) {
  unsigned c = __VERIFIER_nondet_uint();
  unsigned s = s + 1;
  unsigned a = id(a);
  unsigned t = c ? id(c) : 0;
  unsigned u = ({ unsigned k = 0; while (k < c) k = k + 1; k; });
  __VERIFIER_assert(t <= c); // t
  return 0;
}
// Imported from ESBMC regression/goto2lean/uninitialised_reads
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  ∀ s : BitVec 32,\n  let s := \(s \+ \(1#32\)\);$
// EXPECT: ^  ∀ a : BitVec 32,\n  id a \(fun
// EXPECT: ^  ∀ t : BitVec 32,\n  ∀ «tmp\$4» : BitVec 32,$
// EXPECT: ^    ∀ u : BitVec 32,\n    ∀ «tmp\$5» : BitVec 32,$
// EXPECT: ^      let u := «tmp\$5»;$
// Vereda: verdict none
