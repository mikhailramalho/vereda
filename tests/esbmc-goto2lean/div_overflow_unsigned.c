unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint(), y = __VERIFIER_nondet_uint();
  unsigned q = x / y;
  return 0;
}
// Imported from ESBMC regression/goto2lean/div_overflow_unsigned
// Needs: --goto2lean
// RUN: --goto2lean --unsigned-overflow-check --no-div-by-zero-check
// MODE: FUTURE
// EXPECT: ^  ¬\(\(y == 0#32\) = true\) ∧ \($
// Vereda: verdict none
