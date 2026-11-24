// An assumption or branch that makes a value equal to a constant puts the
// constant in its place for the rest of the path (#120): every check below
// folds to false while the walk builds it, before the solver sees it. The
// forms: x == c, a conjunction, !(x != c), assume_abort_if_not's branch in
// a callee, a switch case, and a global loaded again.
// RUN: --dump-smt-only
// EXPECT: ^\(assert false\)\n\(check-sat\)
extern int __VERIFIER_nondet_int(void);
extern void __VERIFIER_assume(int);
extern void __VERIFIER_assert(int);
extern void reach_error(void);
void assume_abort_if_not(int cond) {
  if (!cond)
    __builtin_abort();
}
int g;
int main(void) {
  int x = __VERIFIER_nondet_int(), y = __VERIFIER_nondet_int();
  int z = __VERIFIER_nondet_int(), w = __VERIFIER_nondet_int();
  __VERIFIER_assume(x == 5);
  __VERIFIER_assert(x * 3 + 1 == 16);
  if (x < 4)
    reach_error();
  __VERIFIER_assume(y == 7 && z == -2);
  __VERIFIER_assert(y * z == -14);
  __VERIFIER_assume(!(w != 9));
  __VERIFIER_assert(w % 4 == 1);
  int v = __VERIFIER_nondet_int();
  assume_abort_if_not(v == 100);
  __VERIFIER_assert(v / 10 == 10);
  int s = __VERIFIER_nondet_int();
  switch (s) {
  case 3: __VERIFIER_assert(s * s == 9); break;
  default: break;
  }
  g = __VERIFIER_nondet_int();
  __VERIFIER_assume(g == 42);
  __VERIFIER_assert(g * g == 1764);
  return 0;
}
