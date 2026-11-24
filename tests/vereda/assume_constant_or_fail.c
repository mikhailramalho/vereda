// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT: ^  SUCCESS +\[main\.assertion\.1\] +line 16
// EXPECT: ^  FAILED +\[main\.assertion\.2\] +line 18
// A negated disjunction fixes both sides, x != 5 || y == 6 fixes neither:
// a path may have x anything when y is 6 (#120).
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  int a = __VERIFIER_nondet_int(), b = __VERIFIER_nondet_int();
  if (__VERIFIER_nondet_int())
    return 0;
  if (!(a != 5 || b != 6))
    __VERIFIER_assert(a * b == 30);
  __VERIFIER_assume(x != 5 || y == 6);
  __VERIFIER_assert(x != 5);
  return 0;
}
