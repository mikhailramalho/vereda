// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// The facts a path learns beyond #120's ranges settle these checks as the
// walk builds them (#296), so every reach_error below is pruned: x = y
// between two variables (each bounded as the other is, including by what
// is learnt of either later), a bound on an extension that bounds its
// operand, sext(x) = c fixing x, x != c moving an end of x's range, a
// switch's default and a case that no value in range takes, and a load
// that reads past a store whose index it cannot meet. The callees keep
// InstCombine from rewriting the comparisons across them.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern unsigned char __VERIFIER_nondet_uchar(void);
extern short __VERIFIER_nondet_short(void);
extern void __VERIFIER_assume(int);
extern void reach_error(void);
static int widen(unsigned char c) { return c; }
static long long wide(short s) { return s; }
static void nonzero(unsigned v) {
  if (v == 0)
    reach_error();
}
static void atLeast2(unsigned char k) {
  if (k < 2)
    reach_error();
}
int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  __VERIFIER_assume(x == y);
  __VERIFIER_assume(y < 10);
  if (x >= 10)
    reach_error();
  __VERIFIER_assume(x > 0);
  if (y <= 0)
    reach_error();

  unsigned char c = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(widen(c) < 7);
  if (c >= 7)
    reach_error();
  short s = __VERIFIER_nondet_short();
  if (wide(s) == -300)
    if (s != -300)
      reach_error();

  unsigned u = __VERIFIER_nondet_uint();
  if (u != 0)
    nonzero(u);

  unsigned char k = __VERIFIER_nondet_uchar();
  __VERIFIER_assume(k < 4);
  switch (k) {
  case 0:
  case 1:
    break;
  case 9:
    reach_error();
    break;
  default:
    atLeast2(k);
  }

  int a[10];
  int i = __VERIFIER_nondet_int();
  __VERIFIER_assume(i >= 0 && i < 5);
  a[i] = 1;
  a[i + 5] = 2;
  if (a[i] != 1)
    reach_error();
  return 0;
}
