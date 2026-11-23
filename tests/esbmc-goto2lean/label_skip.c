unsigned __VERIFIER_nondet_uint(void);
void v(unsigned c) {
  if (c) goto L;
  c = 2;
L:;
}
int main(void) {
  unsigned c = __VERIFIER_nondet_uint();
  while (c) { if (c == 3) continue; c = c - 1; }
  v(c);
  return 0;
}
// Imported from ESBMC regression/goto2lean/label_skip
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^def v \(c : BitVec 32\) \(post : Unit → Prop\) : Prop :=\n  let join_1 := \(\n    post \(\)\);$
// Vereda: verdict none
