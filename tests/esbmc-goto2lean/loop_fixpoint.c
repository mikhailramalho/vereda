unsigned __VERIFIER_nondet_uint(void);
int main(void) {
  unsigned n = __VERIFIER_nondet_uint();
  unsigned s = 0, i = 0;
  while (i < n) {
    s = s + 1;
    i = i + 1;
  }
  __VERIFIER_assert(s == n); // s counts the iterations
  return 0;
}
// Imported from ESBMC regression/goto2lean/loop_fixpoint
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^def main_loop1 \(n : BitVec 32\) \(s : BitVec 32\) \(i : BitVec 32\)$
// EXPECT: ^coinductive_fixpoint$
// EXPECT: ^  main_loop1 n s i post ret\)$
// EXPECT: ^  main_loop1 n s i \(fun \(n : BitVec 32\) \(s : BitVec 32\) \(i : BitVec 32\) =>$
// EXPECT: each loop needs an invariant
// EXPECT: ^  sorry$
// Vereda: verdict none
