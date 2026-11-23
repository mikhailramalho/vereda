unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  __VERIFIER_assert(x == x); // refl
  return 0;
}
// Imported from ESBMC regression/goto2lean/output_unwritable
// Needs: --goto2lean --output missing/out.lean
// RUN: --goto2lean --output missing/out.lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: cannot write missing/out\.lean$
// Vereda: verdict none
