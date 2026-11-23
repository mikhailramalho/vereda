int main(void) {
  unsigned x = 1;
  unsigned long a = (unsigned long)&x;
  return 0;
}
// Imported from ESBMC regression/goto2lean/unsupported_pointer_cast
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported cast from pointer to unsignedbv$
// Vereda: verdict none
