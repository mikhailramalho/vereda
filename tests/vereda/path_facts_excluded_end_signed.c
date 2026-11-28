// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// x is in [-5, 5], across zero, so only its signed bounds are tight; it is
// not -4 or 4, so x != -5 moves its lower end to -4 and on to -3, and
// x != 5 its upper end to 4 and on to 3. The checks fold to false as the
// walk builds them (#467).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  if (x < -5 || x > 5)
    return 0;
  if (x == -4 || x == 4)
    return 1;
  if (x == -5 || x == 5)
    return 2;
  __VERIFIER_assert(x >= -3);
  __VERIFIER_assert(x <= 3);
  return 3;
}
