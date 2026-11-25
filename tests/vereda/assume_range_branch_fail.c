// RUN: --multi-property
// EXPECT: ^VERIFICATION FAILED$
// EXPECT-NOT: SUCCESS +\[main\.assertion
// EXPECT: ^  FAILED +\[main\.assertion\.11\]
// assume_range_branch with each check reachable at the edge of its range,
// so a bound one off, or a range that leaks into the other side of a
// branch or across a join, would lose a failure (#120).
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
void assume_abort_if_not(int cond) {
  if (!cond)
    __builtin_abort();
}
int main(void) {
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a <= 10);
  __VERIFIER_assert(!(a > 9));
  __VERIFIER_assume(-3 <= a);
  __VERIFIER_assert(!(a < -2));

  unsigned u = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u < 100);
  __VERIFIER_assert(!(u + 5 > 103));

  int b = __VERIFIER_nondet_int();
  if (b > 7)
    return 0;
  __VERIFIER_assert(!(b >= 7));
  unsigned v = __VERIFIER_nondet_uint();
  if (v < 50)
    return 0;
  __VERIFIER_assert(!(v < 51));

  int c = __VERIFIER_nondet_int();
  __VERIFIER_assume(c > 0 && c < 5);
  __VERIFIER_assert(!(c == 4));
  __VERIFIER_assert(!((unsigned)c > 3u));
  long long w = c;
  __VERIFIER_assert(!(w * 1 > 3 || w < 2));

  int m = __VERIFIER_nondet_int();
  if (__VERIFIER_nondet_int())
    __VERIFIER_assume(m < 3);
  else
    __VERIFIER_assume(m >= 0 && m < 5);
  __VERIFIER_assert(!(m > 3));
  __VERIFIER_assert(!(m < 0));

  int q = __VERIFIER_nondet_int();
  assume_abort_if_not(q < 10);
  __VERIFIER_assert(!(q >= 9));
  return 0;
}
