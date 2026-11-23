void g(unsigned);
void h(void) { }
int main(void) {
  g(1);
  h();
  h();
  __VERIFIER_assert(1); // trivial
  __VERIFIER_assume(1);
  return 0;
}
// Imported from ESBMC regression/goto2lean/void_callee
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^def h \(post : Unit → Prop\) : Prop :=\n  post \(\)$
// EXPECT: ^  -- call to g \(no body\): ignored$
// EXPECT: ^  True ∧ \(\n  True →$
// EXPECT: ^  simp only \[Prog\.h, Prog\.main\]$
// Vereda: verdict none
