int __VERIFIER_nondet_int(void);
int clamp(int x, int lo, int hi) {
  if (x < lo)
    return lo;
  if (x > hi)
    return hi;
  return x;
}
int sign(int x) {
  int s;
  if (x > 0)
    s = 1;
  else if (x < 0)
    s = -1;
  else
    s = 0;
  return s;
}
int main(void) {
  int x = __VERIFIER_nondet_int(), lo = __VERIFIER_nondet_int(), hi = __VERIFIER_nondet_int();
  __VERIFIER_assume(lo <= hi);
  int c = clamp(x, lo, hi);
  __VERIFIER_assert(lo <= c && c <= hi); // clamp in range
  __VERIFIER_assert(sign(c - c) == 0); // sign of zero
  return 0;
}
// Imported from ESBMC regression/goto2lean/branch_join
// Needs: --goto2lean
// RUN: --goto2lean
// MODE: FUTURE
// EXPECT: ^  let join_1 := \(fun \(x : BitVec 32\) \(hi : BitVec 32\) =>$
// EXPECT: ^    \(\(BitVec\.slt hi x = true\) →$
// EXPECT: ^def sign \(x : BitVec 32\) \(post : BitVec 32 → Prop\) : Prop :=$
// EXPECT: ^      let s := \(-\(1#32\)\);$
// EXPECT: ^  \(BitVec\.sle lo hi = true\) →$
// Vereda: verdict none
