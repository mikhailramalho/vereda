int main(void) {
  __VERIFIER_atomic_end();
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_atomic_end
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported instruction type 11 at .*unsupported_atomic_end\.c:2$
// Vereda: verdict none
