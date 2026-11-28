// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// k is -3 on every path that goes on, and both branches return when y, x
// widened with its sign, is k, so sext(x) != -3 holds after the join; the
// path facts carry the excluded -3 from the extension to x, so the check
// on x folds to false as the walk builds it (#468).
extern signed char __VERIFIER_nondet_char(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  signed char x = __VERIFIER_nondet_char();
  int k = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  if (k != -3)
    return 0;
  int y = x;
  if (c) {
    if (y == k)
      return 1;
  } else {
    if (y == k)
      return 2;
  }
  __VERIFIER_assert(x != -3);
  return 3;
}
