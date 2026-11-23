unsigned __VERIFIER_nondet_uint(void);
int main(void){ unsigned _ = __VERIFIER_nondet_uint(); __VERIFIER_assert(_ == _); return 0; } // k
// Imported from ESBMC regression/goto2lean/keyword_identifier
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  ∀ «_» : BitVec 32,$
// EXPECT: ^  \(«_» = «_»\) ∧ \($
// Vereda: verdict none
