// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
// A comparison that the range an assumption or a branch gives a value
// settles folds on that path (#120), so every reach_error below is pruned
// while the walk builds it: signed and unsigned, both directions, the else
// side of a branch, a conjunction, a value an operation or extension
// carries the range through, = against a constant outside it, a join of
// two narrowed paths, and assume_abort_if_not's branch in a callee.
extern int __VERIFIER_nondet_int(void);
extern unsigned __VERIFIER_nondet_uint(void);
extern void __VERIFIER_assume(int);
extern void reach_error(void);
void assume_abort_if_not(int cond) {
  if (!cond)
    __builtin_abort();
}
int main(void) {
  int a = __VERIFIER_nondet_int();
  __VERIFIER_assume(a <= 10);
  if (a > 20)
    reach_error();
  __VERIFIER_assume(-3 <= a);
  if (a < -5)
    reach_error();

  unsigned u = __VERIFIER_nondet_uint();
  __VERIFIER_assume(u < 100);
  if (u + 5 > 104)
    reach_error();

  int b = __VERIFIER_nondet_int();
  if (b > 7)
    return 0;
  if (b >= 9)
    reach_error();
  unsigned v = __VERIFIER_nondet_uint();
  if (v < 50)
    return 0;
  if (v < 40)
    reach_error();

  int c = __VERIFIER_nondet_int();
  __VERIFIER_assume(c > 0 && c < 5);
  if (c == 7)
    reach_error();
  if ((unsigned)c > 4u)
    reach_error();
  long long w = c;
  if (w * 1 > 4 || w < 1)
    reach_error();

  int m = __VERIFIER_nondet_int();
  if (__VERIFIER_nondet_int())
    __VERIFIER_assume(m < 3);
  else
    __VERIFIER_assume(m >= 0 && m < 5);
  if (m > 6)
    reach_error();

  int q = __VERIFIER_nondet_int();
  assume_abort_if_not(q < 10);
  if (q >= 12)
    reach_error();
  return 0;
}
