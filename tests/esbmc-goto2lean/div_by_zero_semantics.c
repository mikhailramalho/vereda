unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint(), y = __VERIFIER_nondet_uint();
  __VERIFIER_assume(y == 0 && x == 7);
  unsigned q = x / y;
  __VERIFIER_assert(q == 0); // q
  return 0;
}
// Imported from ESBMC regression/goto2lean/div_by_zero_semantics
// Needs: --goto2lean
// RUN: --goto2lean --no-div-by-zero-check
// MODE: FUTURE
// EXPECT: ^  let q := \(BitVec\.smtUDiv x y\);$
// Vereda: verdict none
