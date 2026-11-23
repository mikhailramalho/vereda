int main(void) { int a[2] = {1, 2}; int *p = a; __VERIFIER_assert(p[1] == 2); return 0; } // x
// Imported from ESBMC regression/goto2lean/unsupported_array
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported type array$
// Vereda: verdict none
