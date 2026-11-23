unsigned g;
int main(void) { g = 1; __VERIFIER_assert(g == 1); return 0; } // g
// Imported from ESBMC regression/goto2lean/unsupported_global_variable
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^ERROR: goto2lean: unsupported global variable c:@g at .*unsupported_global_variable\.c:1$
// Vereda: verdict none
