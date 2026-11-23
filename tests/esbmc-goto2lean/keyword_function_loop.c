int end(int n) { int i = 0; while (i < n) i = i + 1; return i; }
int main() { int r = end(3); return 0; }
// Imported from ESBMC regression/goto2lean/keyword_function_loop
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^def end_loop1 \(n : BitVec 32\) \(i : BitVec 32\)$
// EXPECT: ^def «end» \(n : BitVec 32\) \(post : BitVec 32 → Prop\) : Prop :=$
// Vereda: verdict none
