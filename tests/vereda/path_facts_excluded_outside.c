// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// One branch excludes 7 from x and the other bounds x to 10..20, so x is
// not 7 after the join either way, and the check folds (#367).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  if (c) {
    if (x == 7)
      return 0;
  } else {
    if (x < 10 || x > 20)
      return 1;
  }
  __VERIFIER_assert(x != 7);
  return 3;
}
