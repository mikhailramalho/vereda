// RUN: --overflow-check --unsigned-overflow-check --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// An overflow check whose operands' ranges keep the result in range folds
// away on the path that bounds them (#296): signed and unsigned addition,
// subtraction and multiplication.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  __VERIFIER_assume(x > -1000 && x < 1000);
  __VERIFIER_assume(y > -1000 && y < 1000);
  int r = x + y;
  r = r - y;
  r = r * 3;
  unsigned u = __VERIFIER_nondet_uint(), v = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u >= 1000 && u < 2000 && v < 1000);
  unsigned w = u + v;
  w = w * v;
  w = u - v;
  return r + (int)w;
}
