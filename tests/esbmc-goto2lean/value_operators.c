unsigned __VERIFIER_nondet_uint(void);
int __VERIFIER_nondet_int(void);
int main(void) {
  unsigned a = __VERIFIER_nondet_uint(), b = __VERIFIER_nondet_uint();
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  unsigned m = a * b;
  unsigned um = a % b;
  int sm = x % y;
  unsigned bn = ~a;
  unsigned ba = a & b, bo = a | b, bx = a ^ b;
  unsigned sl = a << 1, sr = a >> 1;
  int as = x >> 1;
  unsigned t = a ? a : b;
  _Bool c = a < b;
  int lt = x < y, le = x <= y, gt = x > y, ge = x >= y;
  int ult = a < b, ule = a <= b, ugt = a > b, uge = a >= b;
  int eq = a == b, ne = a != b;
  int nt = !x, an = x && y, orr = x || y;
  long wide = x;
  unsigned long uw = a;
  unsigned char nar = a;
  unsigned same = x;
  _Bool tb = 1;
  __VERIFIER_assert(a != b || 1); // ne
  return 0;
}
// Imported from ESBMC regression/goto2lean/value_operators
// Needs: --goto2lean
// RUN: --goto2lean --no-div-by-zero-check
// MODE: FUTURE
// EXPECT: ^  let m := \(a \* b\);$
// EXPECT: ^  let um := \(a % b\);$
// EXPECT: ^  let sm := \(BitVec\.srem x y\);$
// EXPECT: ^  let bn := \(~~~a\);$
// EXPECT: ^  let ba := \(a &&& b\);$
// EXPECT: ^  let bo := \(a \|\|\| b\);$
// EXPECT: ^  let bx := \(a \^\^\^ b\);$
// EXPECT: ^  let sl := \(a <<< \(1#32\)\);$
// EXPECT: ^  let sr := \(a >>> \(1#32\)\);$
// EXPECT: ^  let as := \(BitVec\.sshiftRight' x \(1#32\)\);$
// EXPECT: ^  let t := \(if \(a != 0#32\) then a else b\);$
// EXPECT: ^  let c := \(BitVec\.ult a b\);$
// EXPECT: ^  let lt := \(if \(BitVec\.slt x y\) then 1#32 else 0#32\);$
// EXPECT: ^  let le := \(if \(BitVec\.sle x y\) then 1#32 else 0#32\);$
// EXPECT: ^  let gt := \(if \(BitVec\.slt y x\) then 1#32 else 0#32\);$
// EXPECT: ^  let ge := \(if \(BitVec\.sle y x\) then 1#32 else 0#32\);$
// EXPECT: ^  let ule := \(if \(BitVec\.ule a b\) then 1#32 else 0#32\);$
// EXPECT: ^  let ugt := \(if \(BitVec\.ult b a\) then 1#32 else 0#32\);$
// EXPECT: ^  let uge := \(if \(BitVec\.ule b a\) then 1#32 else 0#32\);$
// EXPECT: ^  let eq := \(if \(a == b\) then 1#32 else 0#32\);$
// EXPECT: ^  let ne := \(if \(a != b\) then 1#32 else 0#32\);$
// EXPECT: ^  let nt := \(if \(!\(x != 0#32\)\) then 1#32 else 0#32\);$
// EXPECT: ^  let an := \(if \(\(x != 0#32\) && \(y != 0#32\)\) then 1#32 else 0#32\);$
// EXPECT: ^  let orr := \(if \(\(x != 0#32\) \|\| \(y != 0#32\)\) then 1#32 else 0#32\);$
// EXPECT: ^  let wide := \(BitVec\.signExtend 64 x\);$
// EXPECT: ^  let uw := \(BitVec\.setWidth 64 a\);$
// EXPECT: ^  let nar := \(BitVec\.setWidth 8 a\);$
// EXPECT: ^  let same := x;$
// EXPECT: ^  let tb := true;$
// EXPECT: ^  \(\(a ≠ b\) ∨ True\) ∧ \($
// Vereda: verdict none
