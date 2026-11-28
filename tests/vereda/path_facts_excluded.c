// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// Both branches return when x is 1, a value inside x's range, so x != 1
// holds after the join, whose guard is a disjunction the check's condition
// is not a conjunct of. The path facts keep 1 as a constant x is not, past
// the join and past the later bound x <= 100, so the check folds to
// false as the walk builds it (#367).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int();
  int c = __VERIFIER_nondet_int();
  if (c) {
    if (x == 1)
      return 0;
  } else {
    if (x == 1)
      return 1;
  }
  if (x > 100)
    return 2;
  __VERIFIER_assert(x != 1);
  return 3;
}
