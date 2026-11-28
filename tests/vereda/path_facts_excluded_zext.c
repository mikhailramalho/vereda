// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// k is 5 on every path that goes on, and both branches return when y, x
// widened, is k, so zext(x) != 5 holds after the join; the path facts
// carry the excluded 5 from the extension to x, so the check on x folds
// to false as the walk builds it (#468).
extern unsigned char __VERIFIER_nondet_uchar(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned char x = __VERIFIER_nondet_uchar();
  int k = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  if (k != 5)
    return 0;
  int y = x;
  if (c) {
    if (y == k)
      return 1;
  } else {
    if (y == k)
      return 2;
  }
  __VERIFIER_assert(x != 5);
  return 3;
}
