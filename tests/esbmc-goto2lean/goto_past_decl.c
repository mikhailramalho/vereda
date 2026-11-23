unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned c = __VERIFIER_nondet_uint();
  unsigned d = __VERIFIER_nondet_uint();
  if (d) goto L;
  goto L;
  unsigned x;
L:
  x = 0;
  if (c) x = 1;
  __VERIFIER_assert(x == 0); // x is zero
  return 0;
}
// Imported from ESBMC regression/goto2lean/goto_past_decl
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^    let join_2 := \(fun \(x : BitVec 32\) =>$
// EXPECT: ^      join_2 x\) ∧$
// Vereda: verdict none
