int __VERIFIER_nondet_int(void);
enum { NEG = -3 };
int main(void) {
  int x = __VERIFIER_nondet_int();
  int k = NEG;
  switch (x) { case -1: k = 1; break; default: break; }
  __VERIFIER_assert(k != 0); // k
  return 0;
}
// Imported from ESBMC regression/goto2lean/negative_constant
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  let k := \(BitVec\.ofInt 32 \(-3\)\);$
// Vereda: verdict none
