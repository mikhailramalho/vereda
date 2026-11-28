// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// x is not 5, 6 or 7, so !(x <= 4) moves its lower end to 5, which is
// excluded, and on to 8. y is not 1 or 2 and its range crosses zero, so
// only its signed bounds are tight: y < 3 moves its upper end to 2 and on
// to 0. Both checks fold to false as the walk builds them (#473).
extern unsigned __VERIFIER_nondet_uint(void);
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  unsigned x = __VERIFIER_nondet_uint();
  int y = __VERIFIER_nondet_int();
  if (x == 5 || x == 6 || x == 7 || y == 1 || y == 2)
    return 1;
  if (x <= 4 || y >= 3)
    return 2;
  __VERIFIER_assert(x >= 8);
  __VERIFIER_assert(y <= 0);
  return 3;
}
