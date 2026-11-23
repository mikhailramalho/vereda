unsigned __VERIFIER_nondet_uint(void);
unsigned f(unsigned x) { if (x) return 1; __builtin_unreachable(); }
int main(void) { unsigned r = f(__VERIFIER_nondet_uint()); return 0; }
// Imported from ESBMC regression/goto2lean/unreachable_fall_off
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^    False →\n    ∀ result : BitVec 32, post result\)\);$
// Vereda: verdict none
