int main(void) {
  unsigned x = 1;
  __VERIFIER_assert(&x); // address as a guard
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_guard_cast
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported cast from pointer to bool$
// Vereda: verdict none
